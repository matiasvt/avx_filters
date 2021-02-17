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

extern int cociente_avx(memoria* mem, int shots,int binsPorLoop,int offset_bines,int cantLoop);
extern int cocientes_sse(memoria* mem, int shots,int binsPorLoop,int offset_bines,int cantLoop);

int F1(short*datos ,int* res_i,long*res,double*res_d,double*res_d2,int bins,int shots){
		memset(res_i,0,bins*sizeof(int));
		memset(res,0,bins*sizeof(long));
		memset(res_d,0,(bins/2)*sizeof(double));
		memset(res_d2,0,(bins/2)*sizeof(double));
		int i;
		for(i=0;i<bins*shots;i+=2){
			short tmp=datos[i]>>2;
			short tmp2=datos[i+1]>>2;
			res[i%bins]+=(tmp*tmp);
			res_i[i%bins]+=tmp;
			res[(i%bins)+1]+=(tmp2*tmp2);
			res_i[(i%bins)+1]+=tmp2;
			if (tmp2!=0) {
				double cociente=(double)(tmp)/(double)(tmp2);
				res_d[(i/2)%(bins/2)]+=cociente;
				res_d2[(i/2)%(bins/2)]+=cociente*cociente;
			}
		}
return 0;
}


int main(int argc,char** argv){

	int iteraciones=5;


	//int bins=80;
	//int shots=100;
	int bins=atoi(argv[1]);
	int shots=atoi(argv[2]);
	
	short* datos;
	
	int* sumas;
	int* sumas_sse;
	int* sumas_c=malloc(bins*sizeof(int));

	long* sumas2;
	long* sumas2_sse;
	long* sumas2_c=malloc(bins*sizeof(long));

	double* cociente;
	double* cociente_sse;
	double* cociente_c=malloc(bins*sizeof(double)/2);
	double* cociente2;
	double* cociente2_sse;
	double* cociente2_c=malloc(bins*sizeof(double)/2);


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


	if (posix_memalign((void **)&cociente, align, bins*sizeof(double)/2)<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}
	if (posix_memalign((void **)&cociente_sse, 16, bins*sizeof(double)/2)<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}

	if (posix_memalign((void **)&cociente2, align, bins*sizeof(double)/2)<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}
	if (posix_memalign((void **)&cociente2_sse, 16, bins*sizeof(double)/2)<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}


	srand(time(NULL));
	//int i;
			  FILE* ifile=fopen(argv[3],"rb");
  int leidos=fread(datos,sizeof(short),bins*shots,ifile);
  if(leidos<0) return 1;
/*	for (i=0;i<bins*shots;++i) {
		//datos[i]=4;
		//datos[i]=(i%(bins/2))*4;
		//if(i%2==0)datos[i]=8;
		datos[i]=(short)(rand()%8192)*4;
	}
*/

	memoria param; //struct con punteros

	param.datos=datos;
	param.sumas=sumas;
	param.sumas2=sumas2;
	param.cocientes=cociente;
	param.cocientes2=cociente2;


	memoria param2; //struct con punteros

	param2.datos=datos;
	param2.sumas=sumas_sse;
	param2.sumas2=sumas2_sse;
	param2.cocientes=cociente_sse;
	param2.cocientes2=cociente2_sse;


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
		memset(param.cocientes,0,bins*sizeof(double)/2);	
		memset(param.cocientes2,0,bins*sizeof(double)/2);	

			cociente_avx(&param,shots,binsPorLoop,offset_bines,bins/binsPorLoop);
		clock_gettime(CLOCK_MONOTONIC, &end);
		ts+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
		memcpy(chc,chb,1024*1024*20*4); //cachce_flush

		clock_gettime(CLOCK_MONOTONIC, &start);
		memset(param2.sumas,0,bins*sizeof(int));
		memset(param2.sumas2,0,bins*sizeof(long));	
		memset(param2.cocientes,0,bins*sizeof(double)/2);	
		memset(param2.cocientes2,0,bins*sizeof(double)/2);

			cocientes_sse(&param2,shots,binsPorLoop_sse,offset_bines,bins/binsPorLoop_sse);
		clock_gettime(CLOCK_MONOTONIC, &end);
		ts1+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
		memcpy(chc,chb,1024*1024*20*4);


		clock_gettime(CLOCK_MONOTONIC, &start);
		F1(datos,sumas_c,sumas2_c,cociente_c,cociente2_c,bins,shots);
		clock_gettime(CLOCK_MONOTONIC, &end);
		        memcpy(chc,chb,1024*1024*20*4);

		ts2+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
iteraciones--;
}
printf("%.5f %.5f %.5f",(double)ts1/50.0,(double)ts/50.0,(double)ts2/50.0);
if(argc>4){
FILE* s=fopen(argv[4],"wb");
FILE* s2=fopen(argv[5],"wb");
if (s)fwrite(param.cocientes,sizeof(double),bins/2,s);
if (s2)fwrite(param.cocientes2,sizeof(double),bins/2,s2);
fclose(s);fclose(s2);
}
printf("SSE tarda %f del tiempo que C para normalizar\nAVX tarda %f del tiempo que C para normalizar\n",(double)ts1/(double)ts2,(double)ts/(double)ts2);

/*
double maxError=0.0;
double maxError2=0.0;

double maxError_c=0.0;
double maxError2_c=0.0;

	for(i=0;i<bins;++i){ //chequeo errores en el código
	//	printf("%ld - %ld\n",param.sumas2[i],param2.sumas2[i]);
		if(param.sumas[i]!=param2.sumas[i]) printf("Error al sumar !\n");
		if(param.sumas[i]!=sumas_c[i]) printf("Error al sumar  contra C!\n");
		if(param.sumas2[i]!=param2.sumas2[i]) printf("Error al sumar al cuadrado!\n");
		if(param.sumas2[i]!=sumas2_c[i]) printf("Error al sumar al cuadrado contra C!\n");
		double error=fabs(param.cocientes[i/2]-param2.cocientes[i/2]);
		double error2=fabs(param.cocientes2[i/2]-param2.cocientes2[i/2]);
		double error_c=fabs(param.cocientes[i/2]-cociente_c[i/2]);
		double error2_c=fabs(param.cocientes2[i/2]-cociente2_c[i/2]);
		if (error>maxError) maxError=error;
		if (error2>maxError2) maxError2=error2;

		if (error_c>maxError_c) maxError_c=error_c;
		if (error2_c>maxError2_c) maxError2_c=error2_c;
	}

	printf("Errores Maximos: %f y %f\n",maxError,maxError2);
		printf("Errores Maximos contra C: %f y %f\n",maxError_c,maxError2_c);
*/

	free(datos);

	free(sumas);
	free(sumas_sse);
	free(sumas_c);

	free(sumas2);
	free(sumas2_sse);
	free(sumas2_c);
	
	free(cociente);
	free(cociente2);

	free(cociente_sse);
	free(cociente2_sse);

	free(cociente2_c);free(cociente_c);

	free(chc);free(chb);

	return 0;
}
