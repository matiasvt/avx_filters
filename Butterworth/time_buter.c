#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdint.h>


/*;rdi:m
;rdi+8:mf
;rdi+16:a
;rdi+24:b
;rdi+32:coef
;rdi+36:tamFila
;rdi+40:offsetBines
;rdi+44:shots*/

typedef struct {
	double* m;
	double* mf;
	double* a;
	double* b;
	int coef;
	int tamFila;
	int offsetBines;
	int shots;
} memoria;

extern int buter(memoria* param);


void filtro2d(double* matrix, unsigned int shots, unsigned int bins, double* a, unsigned int size_a, double* b, double* matrix_f){


    int i;
    int j;
    int k;
//    for (i=0;i<size_a;++i){
  //      a[i] = a[i]/a[0];
   // }

    //a[0] = .0;

for(j=0;j<size_a;++j){

    for(i=0;i<bins;++i){
        matrix_f[i+j*bins]=matrix[i+j*bins]*b[0];
    }

    if (j>0){
        for(k=1;k<j+1;k++){
            for(i=bins*j;i<bins*(j+1);++i){
                matrix_f[i]+=b[k]*matrix[i-k*bins]-a[k]*matrix_f[i-k*bins];
            }
        }
    }
}

    for(i=bins*size_a;i<bins*shots;i+=bins){
        for(j=i;j<i+bins;++j){
            matrix_f[j]=matrix[j]*b[0];
        }
        for(k=size_a-1;k>0;--k){
            for(j=i;j<i+bins;++j){
                matrix_f[j]+=b[k]*matrix[j-k*bins]-a[k]*matrix_f[j-k*bins];
            }
        } 
    }
}


double** filter_2d_double(double** matrix, unsigned int matrix_size1, unsigned int matrix_size2, double* a, unsigned int size_a, double* b, double** matrix_f){
//filtro implementado sobre matrices con doble indice

    int i;
    for (i=0;i<size_a;++i){
        a[i] = a[i]/a[0];
    }

    a[0] = .0;

    int j;
    int k;
    int lim_for;
    for (k=0;k<matrix_size2;++k){
        for(i=0;i<matrix_size1;++i){

            if (i < size_a-1){
                lim_for = i+1;
            } else {
                lim_for = size_a;
            }

            for (j=0;j<lim_for;++j){
                matrix_f[i][k] = matrix_f[i][k] + b[j]*matrix[i-j][k] - a[j]*matrix_f[i-j][k];
            }
        }
    }
return 0;
}



double **matrix_double(unsigned int nrh,unsigned int nch)
{
    int i, j;
    double **m;

    m = (double **) malloc((nrh) * sizeof(double *));


    for (i = 0; i < nrh; i++) {
        m[i] = (double *) malloc((nch) * sizeof(double));
    }

    for (i = 0; i < nrh; i++)
	for (j = 0; j < nch; j++)
	    m[i][j] = 0.;
    return m;
}


int main(int argc,char** argv)
{


    int size_a = 6; //cantidad de coeficientes
    
   // int matrix_size2 = 2400;//bins
   // int matrix_size1 = 10000;//shots


//    int bins=matrix_size2;
  //  int shots=matrix_size1;

int bins=atoi(argv[1]);
int shots=atoi(argv[2]);



    /// DOUBLE matrix
  //  double** matrixd = matrix_double(shots,bins);
   // double** matrix_fd = matrix_double(matrix_size1,matrix_size2);

    double* ad = malloc(sizeof(double)*size_a);
    double* bd = malloc(sizeof(double)*size_a);

    int i;
    int j;
/*
    for (j=0;j<matrix_size2;++j){ //incializo matriz para probar con buter de matlab
        for (i=0;i<matrix_size1;++i){
            matrixd[i][j] = 105*sin((double)i/2)*sin((double)j/200);
        }
    }

*/
    
    double* m;
    double* mf;
    double* m2;
    double* mf2;
    int align=32;
    if (posix_memalign((void **)&m, align, bins*shots*sizeof(double))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}

	if (posix_memalign((void **)&mf, align, bins*shots*sizeof(double))<0){
		printf("No reservo memoria!\nChau\n");
		return -1;
	}
        if (posix_memalign((void **)&m2, align, bins*shots*sizeof(double))<0){
        printf("No reservo memoria!\nChau\n");
        return -1;
    }

    if (posix_memalign((void **)&mf2, align, bins*shots*sizeof(double))<0){
        printf("No reservo memoria!\nChau\n");
        return -1;
    }
/*	
    for (j=0;j<matrix_size2;++j){
        for (i=0;i<matrix_size1;++i){
            m[i*matrix_size2+j]=matrixd[i][j];
                m2[i*matrix_size2+j]=matrixd[i][j];
        }
    }
*/

FILE* ifile=fopen(argv[3],"rb");
  int leidos=fread(m,sizeof(double),bins*shots,ifile);
  if(leidos<0) return 1;
  
  memcpy(m2,m,bins*shots*sizeof(double));

    for(i=0;i<bins*shots;++i){
    mf[i]=35;
    mf2[i]=45;
}

//printf("Matriz copiada \n");

//coeficientes
    ad[0] = 1.00000000000000;
    ad[1] = -4.898337145711599;
    ad[2] = 9.598497090805596;
    ad[3] =  -9.405307989195730;
    ad[4] =  4.608476358536904;
    ad[5] =  -0.903328285337999;

    bd[0] = 0.950435839674620;
    bd[1] =  -4.752179198373098;
    bd[2] =  9.504358396746197;
    bd[3] =  -9.504358396746197;
    bd[4] = 4.752179198373098;
    bd[5] =  -0.950435839674620;



int iteraciones,it;
iteraciones=it=5;

    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC, &start);
while(it){
  //  filter_2d_double(matrixd, matrix_size1, matrix_size2, ad, size_a, bd, matrix_fd);
    filtro2d(m2,shots,bins,ad,size_a,bd,mf2);
    it--;
}
    clock_gettime(CLOCK_MONOTONIC,&end);
unsigned long ts=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
//printf("Tiempo para C  en %d iteraciones: %ld\n",iteraciones,ts);

int binesProcesados=4;//double avx

memoria param;
param.m=m;
param.mf=mf;
param.a=ad;
param.b=bd;
param.coef=size_a;
param.offsetBines=bins*sizeof(double);
int tamFila=bins/binesProcesados;
param.tamFila=tamFila;
param.shots=shots;



it=iteraciones;
    clock_gettime(CLOCK_MONOTONIC, &start);

while(it){
    buter(&param);
    it--;
}
    clock_gettime(CLOCK_MONOTONIC,&end);
unsigned long ts2=1000000000*(end.tv_sec-start.tv_sec)+end.tv_nsec-start.tv_nsec;
printf("%.5f %.5f",(double)ts2/(double)iteraciones,(double)ts/(double)iteraciones);
//printf("Tiempo para AVX  en %d iteraciones: %ld\n",iteraciones,ts2);
//printf("ASM tarda el %.4f por ciento del tiempo\n",100.0*((double)ts2)/(double)ts);
//printf("%p %d\n",mf,rbx);
if(argc>4){
FILE* ofile=fopen(argv[4],"wb");
fwrite(mf,sizeof(double),bins*(shots),ofile);
fclose(ofile);
}
int matrix_size2=bins;

double maxA;
maxA=0.0;


for(j=0;j<bins;++j){
    for(i=0;i<shots;++i){
        double diff=fabs((mf[i*matrix_size2+j]-mf2[i*matrix_size2+j])/mf[i*matrix_size2+j]);
   //     printf("%f\n",diff);
        if(diff>maxA) maxA=diff;
  //      diff=fabs((matrix_f[i][j]-matrix_fd[i][j])/matrix_fd[i][j]);
     //   printf("%f\n",diff);
      //  if(diff>maxF) maxF=diff;
    }
}

//printf("Max Error FMA:%f\n",maxA);

free(m);free(mf);free(m2);free(mf2);free(ad);free(bd);
fclose(ifile);

    return 0;
}




