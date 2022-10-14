#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
//Dichiarazione dei prototipi delle funzioni
void crea_matrice(int  ,int , int ** );
void stampa_matrice(int  ,int  ,int ** );
void vettore_ris(int, int *);
int controllo_threads(int, int);

int main(){
int N, M,i,j;
int **A, *b;
int nloc, p,sum,r,id,step,start,end,tmp,max;
double t1,t0,tp;
srand(time(NULL)); //Inizializzo il seme ad ogni run
printf("Inserisci numero di righe \n");
scanf("%d", &N);
printf("Inserisci numero di colonne \n");
scanf("%d", &M);

//prendo il numero massimo di thread assegnati con export
max=omp_get_max_threads();

//Funzione che ritorna il numero di thread da usare in omp parallel
tmp=controllo_threads(max,M);

//Allocazione dinamica della matrice
A = (int**)malloc(N * sizeof(int*));
         for (i = 0; i < N; i++)
                A[i] = (int*)malloc(M * sizeof(int));

//Allocazione dinamica del vettore
b = (int*)malloc(M * sizeof(int));

//funzione che riempie la matrice
crea_matrice(N,M,A);
//funzione per stampare la matrice
stampa_matrice(N,M,A);

t0=omp_get_wtime();
#pragma omp parallel num_threads(tmp) private(i,j,id,start,end,step,sum) shared(A,b,r) //Inizio codice parallelo
{
        p=omp_get_num_threads();
        nloc=M/p;
        //Implementazione perfetta divisibilità
        r=M%p;
        id=omp_get_thread_num();
	  if (id < r)
        {
                nloc++;
                step=0;
        }
        else
                step=r;
        //Dichiaro inizio e fine sottomatrice
        start=nloc*id+step;
        end=nloc*id+step+nloc-1;

        //Effettuo le somme locali per ogni thread
        for(i=0; i<N; i++){
                for(j=start; j<=end; j++){
                        sum=A[i][j]+sum;
                }
        }
        printf("La sottomatrice che va da colonna: %d a colonna: %d è gestita dal thread: %d \n La somma degli elementi della sottomatrice e': %d \n",start, end, id, sum);
        b[nloc*id]=sum;

} //Fine codice parallelo
t1=omp_get_wtime();
tp=t1-t0;
        vettore_ris(M,b); //Funzione che stampa il vettore risultante
        printf("il tempo trascorso e' : %f \n",tp);
return 0;
}

void crea_matrice(int N, int M, int **A) {
int i,j;
        //riempio la matrice con numeri casuali da 1 a 10
        for (i=0; i<N; i++) {
                for(j=0; j<M; j++){
                        A[i][j]=rand()%10+1;

                }
           }
}

void stampa_matrice(int N, int M, int **A) {
int i,j;
        for (i=0; i<N; i++){
                for(j=0; j<M; j++){
                        printf("%d  ", A[i][j]);
                }
                printf("\n");
        }
 }

void vettore_ris(int M, int *b){
int j;
        for(j=0; j<M; j++){
                printf("Il vettore alla posizione %d risultante e': %d \n", j , b[j]);
        }
}

int controllo_threads(int max, int M) { //Questa funzione setta il giusto numero di threads, se il numero di threads è > colonne, vuol dire che questi sono in eccesso, al più 1 thread gestisce 1 colonna.
int tmpf;
        if(max>M)
                tmpf=M;
        else
                tmpf=max;
return tmpf;
}