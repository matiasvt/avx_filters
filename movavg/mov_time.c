#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>    

#include <math.h>
#include <time.h>//hola

extern void mov_avg_avx(short*m,double*res, int filasRestantes,int offsetBines,double* w);

void mov_avg(short*m,double*res, int w,int bins,int shots){
	int i;
	int j=0;
	int k=bins*w;
	for(i=bins;i<bins*(shots-w+1);++i){
		double old=(double)(m[j]>>2);
		double new=(double)(m[k]>>2);
	res[i]=res[i-bins]+(new-old)/(double)w;
	j++;k++;
	}
}

int main(int argc,char** argv){

	int it=5;

	int w=10; //ventana promedios
	//int bins=8000;
	//int shots=10000;
	int bins=atoi(argv[1]);
	int shots=atoi(argv[2]);

	double* res=calloc(bins*(shots-w+1),sizeof(double));
	short* m = malloc(bins*shots*sizeof(short));


	double* res2;
	short* m2;
	int align=32;
	if (posix_memalign((void **)&m2, align, bins*shots*sizeof(short))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}

	if (posix_memalign((void **)&res2, align, bins*(shots-w+1)*sizeof(double))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}


	int i;
/*
	srand(time(NULL));

	for(i=0;i<bins*shots;++i){

		m[i]=m2[i]=4*(rand()%8192);
	//	m[i]=m2[i]=32;

	//	if (i%2==0)	m[i]=m2[i]=16;
		
	}*/

  FILE* ifile=fopen(argv[3],"rb");
  int leidos=fread(m2,sizeof(short),bins*shots,ifile);
  if(leidos<0) return 1;
  memcpy(m,m2,bins*shots*sizeof(short));
//  printf("%d-%d\n",m[1005],m2[1005]);
///INCICIALIZACION
	memset(res,0,bins*(shots-w+1)*sizeof(double));
	memset(res2,0,bins*(shots-w+1)*sizeof(double));

	for(i=0;i<bins*w;++i){
		res[i%bins]+=(m2[i]>>2);
		res2[i%bins]+=(m2[i]>>2);		
		if((i%bins)==0)		printf("%f\n",res[i%bins]);
	}
	for(i=0;i<bins;++i) {
		res[i]=res2[i]=(double)res[i]/(double)w;
		
	}
/// FIN INCICIALIZACION

	double ventana=(double)w;
	int binesProcesados=8;//proceso 8 cada vez

unsigned long ts,ts2,ts1;
struct timespec start,end;
ts=ts2=ts1=0;

char* chc=malloc(1024*1024*20*4);
char* chb=malloc(1024*1024*20*4);
memset(chb,'a',1024*1024*20*4);

while(it){


		//	memset(res,0,bins*(shots-w+1)*sizeof(double));
		//	memset(res2,0,bins*(shots-w+1)*sizeof(double));

		clock_gettime(CLOCK_MONOTONIC, &start);

			for(i=0;i<bins*w;++i){
				if(i/bins==0){
					res[i%bins]=(m[i]>>2);
					res2[i%bins]=(m[i]>>2);				
				}else{
				res[i%bins]+=(m[i]>>2);
				res2[i%bins]+=(m[i]>>2);		
				}
			}
			for(i=0;i<bins;++i) res[i]=res2[i]=(double)res[i]/(double)w;

		clock_gettime(CLOCK_MONOTONIC, &end);
		ts1+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;


		clock_gettime(CLOCK_MONOTONIC, &start);
			mov_avg_avx(m2,&res2[bins], (shots-w)*bins/binesProcesados,bins,&ventana);
		clock_gettime(CLOCK_MONOTONIC, &end);
		ts+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
		
		memcpy(chc,chb,1024*1024*20*4);

		clock_gettime(CLOCK_MONOTONIC, &start);

			mov_avg(m,res,w,bins,shots);
		clock_gettime(CLOCK_MONOTONIC, &end);
		    memcpy(chc,chb,1024*1024*20*4);

		ts2+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
it--;
}
printf("AVX:%.5f \t C:%.5f",(double)ts/50.0,(double)ts2/50.0);
//printf("mov avg tarda %f del tiempo de C\n",(double)ts/(double)ts2);
//printf("La Inicializacion en C tarda %f del tiempo del filtro optimizado\n",(double)ts1/(double)ts);
if(argc>4){
FILE* ofile=fopen(argv[4],"wb");
fwrite(res2,sizeof(double),bins*(shots-w+1),ofile);
fclose(ofile);
}
	double maxError=0.0;
	for(i=0;i<bins*(shots-w+1);++i){
		//printf("%f -> %f\n",res[i],res2[i]);
		double error=fabs(res[i]-res2[i]);
		if (error>maxError) maxError=error; 
	}

//		printf("Maximo error:%f\n",maxError);

	/*
unsigned long ts,ts2;
struct timespec start,end;

int it=50;
while(it){

	memset(res,0,bins*(shots-w+1)*sizeof(double));
    clock_gettime(CLOCK_MONOTONIC, &start);

	for(i=0;i<bins*w;++i){
		res[i%bins]+=(m[i%bins]>>2);		
	}
	for(i=0;i<bins;++i) res[i]=(double)res[i]/(double)w;
    clock_gettime(CLOCK_MONOTONIC, &end);
 ts+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;


    clock_gettime(CLOCK_MONOTONIC, &start);

	mov_avg(m,res,w,bins,shots);

    clock_gettime(CLOCK_MONOTONIC, &end);
 ts2+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
it--;
}
printf("Inicializacion/filtro: %f\n",(double)ts/(double)ts2);

*/
fclose(ifile);
free(res);free(m);free(res2);free(m2); free(chb);free(chc);

	return 0;
}