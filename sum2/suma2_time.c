#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


typedef struct {
	short* datos;
	int* sumas;
	long* sumas2;
	double* cocientes;
	double* cocientes2;

} memoria;

extern int suma2_avx(memoria* mem, int shots,int binsPorLoop,int offset_bines,int cantLoop);
extern int suma2_sse(memoria* mem, int shots,int binsPorLoop,int offset_bines,int cantLoop);


int F1_suma2(short*datos ,int* res_i,long* res,int bins,int shots){
		memset(res_i,0,bins*sizeof(int));
		memset(res,0,bins*sizeof(long));

		int i;
		for(i=0;i<bins*shots;++i){
			short tmp=(datos[i]>>2);
			res_i[i%bins]+=tmp;
			res[i%bins]+=tmp*tmp;

		}
return 0;
}


int main(int argc,char** argv){

	int iteraciones=5;


//	int bins=800;
//	int shots=10000;
	int bins=atoi(argv[1]);
	int shots=atoi(argv[2]);
	short* datos;
	
	int* sumas;
	int* sumas_sse;
	int* sumas_c=malloc(bins*sizeof(int));

	long* sumas2;
	long* sumas2_sse;
	long* sumas2_c=malloc(bins*sizeof(long));


	int align=32;//avx
	if (posix_memalign((void **)&datos, align, bins*shots*sizeof(short))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}

	if (posix_memalign((void **)&sumas, align, bins*sizeof(int))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}
	if (posix_memalign((void **)&sumas_sse, 16, bins*sizeof(int))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}

	if (posix_memalign((void **)&sumas2, align, bins*sizeof(long))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}
	if (posix_memalign((void **)&sumas2_sse, 16, bins*sizeof(long))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}


	srand(time(NULL));
	//int i;

/*	for (i=0;i<bins*shots;++i) {
		//datos[i]=4;
		//datos[i]=(i%(bins/2))*4;
		//if(i%2==0)datos[i]=8;
		datos[i]=(short)(rand()%8192)*4;
	}*/

		  FILE* ifile=fopen(argv[3],"rb");
  int leidos=fread(datos,sizeof(short),bins*shots,ifile);
  if(leidos<0) return 1;


	memoria param; //struct con punteros

	param.datos=datos;
	param.sumas=sumas;
	param.sumas2=sumas2;

	memoria param2; //struct con punteros

	param2.datos=datos;
	param2.sumas=sumas_sse;
	param2.sumas2=sumas2_sse;

//	param.sumas2=sumas2;
//	param.cocientes=cocientes;
//	param.cocientes2=cocientes2;

	int binsPorLoop=8;
	int binsPorLoop_sse=4;//cantidad de puntos que lee de la matriz por iteracion
	int offset_bines=bins*sizeof(short);


unsigned long ts,ts1,ts2;
struct timespec start,end;
ts=ts1=ts2=0;

char* chc=malloc(1024*1024*20*4);
char* chb=malloc(1024*1024*20*4);
memset(chb,'a',1024*1024*20*4);

while(iteraciones){

		clock_gettime(CLOCK_MONOTONIC, &start);

		memset(param.sumas,0,bins*sizeof(int));	
		memset(param.sumas2,0,bins*sizeof(long));	

			suma2_avx(&param,shots,binsPorLoop,offset_bines,bins/binsPorLoop);
		clock_gettime(CLOCK_MONOTONIC, &end);
		ts+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
		memcpy(chc,chb,1024*1024*20*4);

		clock_gettime(CLOCK_MONOTONIC, &start);
		memset(param2.sumas,0,bins*sizeof(int));
		memset(param2.sumas2,0,bins*sizeof(long));	
	
			suma2_sse(&param2,shots,binsPorLoop_sse,offset_bines,bins/binsPorLoop_sse);
		clock_gettime(CLOCK_MONOTONIC, &end);
		ts1+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
		memcpy(chc,chb,1024*1024*20*4);


		clock_gettime(CLOCK_MONOTONIC, &start);
		F1_suma2(datos,sumas_c,sumas2_c,bins,shots);
		clock_gettime(CLOCK_MONOTONIC, &end);
		        memcpy(chc,chb,1024*1024*20*4);

		ts2+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
iteraciones--;
}
printf("SSE tarda %f del tiempo que C para sumar al cuadrado\nAVX tarda %f del tiempo que C para sumar al cuadrado\n",(double)ts1/(double)ts2,(double)ts/(double)ts2);

//printf("%.5f %.5f %.5f %.5f", (double)ts1/20.0,(double)ts/20.0,(double)ts2/20.0,(double)ts/(double)ts1);
if(argc>4){
FILE* s=fopen(argv[4],"wb");
FILE* s2=fopen(argv[5],"wb");
if (s)fwrite(param.sumas,sizeof(int),bins,s);
if (s2)fwrite(param.sumas2,sizeof(long),bins,s2);
fclose(s);fclose(s2);
}
//int i;
//for(i=0;i<bins;++i) printf("%d -- %ld\n",param.sumas[i],param.sumas2[i]);
/*
	for(i=0;i<bins;++i){ //chequeo errores en el código
	//	printf("%ld - %ld\n",param.sumas2[i],param2.sumas2[i]);
		if(param.sumas[i]!=param2.sumas[i]) printf("Error al sumar !\n");
		if(param.sumas[i]!=sumas_c[i]) printf("Error al sumar  contra C!\n");
		if(param.sumas2[i]!=param2.sumas2[i]) printf("Error al sumar al cuadrado!\n");
		if(param.sumas2[i]!=sumas2_c[i]) printf("Error al sumar al cuadrado contra C!\n");
	}
*/
	fclose(ifile);
	free(datos);

	free(sumas);
	free(sumas_sse);
	free(sumas_c);

	free(sumas2);
	free(sumas2_sse);
	free(sumas2_c);
	//free(sumas2);
	//free(cocientes);
	//	free(cocientes2);



	return 0;
}
