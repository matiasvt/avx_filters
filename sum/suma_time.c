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

extern int suma_avx(memoria* mem, int shots,int binsPorLoop,int offset_bines,int cantLoop);
extern int suma_sse(memoria* mem, int shots,int binsPorLoop,int offset_bines,int cantLoop);


int F1_suma(short*datos ,int* res_i,int bins,int shots){
		memset(res_i,0,bins*sizeof(int));
		int i;
		for(i=0;i<bins*shots;++i){
			res_i[i%bins]+=(datos[i]>>2);
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


	//srand(time(NULL));
//	int i;
	//for (i=0;i<bins*shots;++i) {
		//datos[i]=4;
		//datos[i]=(i%(bins/2))*4;
		//if(i%2==0)datos[i]=8;
	//	datos[i]=(short)(rand()%8192)*4;
	//}
  FILE* ifile=fopen(argv[3],"rb");
  int leidos=fread(datos,sizeof(short),bins*shots,ifile);
  if(leidos<0) return 1;
  printf("%d --- %d\n",datos[1],datos[550]);
	memoria param; //struct con punteros

	param.datos=datos;
	param.sumas=sumas;

	memoria param2; //struct con punteros

	param2.datos=datos;
	param2.sumas=sumas_sse;
//	param.sumas2=sumas2;
//	param.cocientes=cocientes;
//	param.cocientes2=cocientes2;

	int binsPorLoop=8;
	int binsPorLoop_sse=4;//cantidad de puntos que lee de la matriz por iteracion
	int offset_bines=bins*sizeof(short);


unsigned long ts,ts1,ts2,t_acc;
struct timespec start,end;
ts=ts1=ts2=t_acc=0;

char* chc=malloc(1024*1024*20*4);
char* chb=malloc(1024*1024*20*4);
memset(chb,'a',1024*1024*20*4);

while(iteraciones){

		clock_gettime(CLOCK_MONOTONIC, &start);
		memset(param.sumas,0,bins*sizeof(int));	
			suma_avx(&param,shots,binsPorLoop,offset_bines,bins/binsPorLoop);
		clock_gettime(CLOCK_MONOTONIC, &end);
		ts+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
		memcpy(chc,chb,1024*1024*20*4);

		clock_gettime(CLOCK_MONOTONIC, &start);
		memset(param2.sumas,0,bins*sizeof(int));	
			suma_sse(&param2,shots,binsPorLoop_sse,offset_bines,bins/binsPorLoop_sse);
		clock_gettime(CLOCK_MONOTONIC, &end);
		ts1+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
		memcpy(chc,chb,1024*1024*20*4);


		clock_gettime(CLOCK_MONOTONIC, &start);
			F1_suma(datos,sumas_c,bins,shots);
		clock_gettime(CLOCK_MONOTONIC, &end);
		ts2+=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
		memcpy(chc,chb,1024*1024*20*4);

iteraciones--;
}
printf("SSE tarda %f del tiempo que C para sumar\nAVX tarda %f del tiempo que C para sumar\n",(double)ts1/(double)ts2,(double)ts/(double)ts2);
//printf("%.5f %.5f %.5f %.5f", (double)ts1/50.0,(double)ts/50.0,(double)ts2/50.0,(double)t_acc/(double)ts);
if(argc>4){
FILE* ofile=fopen(argv[4],"wb");
if (ofile)fwrite(sumas,sizeof(int),bins,ofile);
fclose(ofile);
}
//int i;
//for(i=0;i<bins;++i)printf("%f\n",(double)param.sumas[i]/(double)shots);
//	for(i=0;i<bins;++i){ //chequeo errores en el código
//		if(param.sumas[i]!=param2.sumas[i]) printf("Error en las sumas!\n");
//		if(param.sumas[i]!=sumas_c[i]) printf("Error en las sumas con C!\n");
//	}

	fclose(ifile);
	free(datos);
	free(sumas);
	free(sumas_sse);
	free(sumas_c);
	//free(sumas2);
	//free(cocientes);
	//	free(cocientes2);



	return 0;
}
