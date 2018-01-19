#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "mpi.h"


#define M 3 //(1<<5)
#define N 5 //(1<<5)
#define P 19 //(1<<5)


//
//initialisation des matrices

void initm(void *m, int ln, int col) {
int i, j;
int * mp = (int *)m;
time_t t;
srand((unsigned) time(&t));




for (i = 0; i < ln; i++) {	
	for (j = 0; j < col; j++) {
		mp[i * col + j] = rand() % 4;
	}
}


}



//afficher une matrices
void printm(void *m, int ln, int col) {

int i, j;
int *mp = (int *)m;

for (i = 0; i < ln; i++) {
	for (j = 0; j < col; j++) {
		printf("%2d ", mp[i * col + j]);		
	
	}
	printf("\n");
}


}


//multiplication sequenciel pour verification
void matmut(void *ma, void *mb, void *mc) {
int i, j, k;
int *pa, *pb, *pc;
pa = (int *)ma;
pb = (int *)mb;
pc = (int *)mc;
	for (i = 0; i < M; i++) {
		for (j = 0; j < P; j++) {
			pc[i * P + j] = 0;
			for (k = 0; k < N; k++) {
			pc[i * P + j] +=  pa[i * N + k] * pb[k * P + j];

			}


		}

	}



}





int main(int argc, char *  argv []) {

int np, rank, mess_s, Tag = 23, Tag_s = 43, i, j, n, k, mess_size;
char message[100];
int ma[Tag_s][N], mb[N][P], mc[M][P], mpic[M][P];
//mc matrice resutlant en sequentiel.
// mpic matrice resultant en parallel.

int tag1 = 1, tag2 = 2, tag3 = 3, tag4 = 4;


MPI_Status stat;

MPI_Init(&argc, &argv);

MPI_Comm_size(MPI_COMM_WORLD, &np);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);


if (np <= 1) {
	printf("Pas de parallelisme \n");
	initm(ma, M, N);
printm(ma, M, N);

printf("\n * \n");
initm(mb, N, P);
printm(mb, N, P);

printf(" \n ==========\n");
printf("Multiplication sequentiel. \n\n");
matmut(ma, mb, mc);
printm(mc, M, P);

printf(" \n ==========\n\n");


}
else {






//master
if (rank == 0) {




//initialisation
initm(ma, M, N);
printm(ma, M, N);

printf("\n * \n");
initm(mb, N, P);
printm(mb, N, P);

printf(" \n ==========\n");
printf("Multiplication sequentiel. \n\n");
matmut(ma, mb, mc);
printm(mc, M, P);

printf(" \n ==========\n\n");





if(P <= np - 1) {  //nombre de colonnes inferieure au nombre de processus

	
//envoi de la matrice A au slaves
	for (i = 0; i < P; i++) {
	MPI_Send(ma, M*N, MPI_INT, i+1, tag1, MPI_COMM_WORLD);
	
	}
}

else { //envoyer a tous
		for (i = 1; i < np; i++) {
	MPI_Send(ma, M*N, MPI_INT, i, tag1, MPI_COMM_WORLD);
	}

}





	//partitionnement de la matrice B a travers les lignes

	int portion, rest; 
	if (P <= np - 1) {
	portion = 1;
	rest = 0;
	} else {
	portion = P / (np - 1);
	rest = P % (np - 1);

	}





	
	mess_s = portion;
	
	
	
	int *mat_b = (int*)mb;
	int * part_p = (int *)malloc(sizeof(int) * portion * N);
	
	int offset;

	//extraction est envoi d'une partie

	int extract;
	if (P <= np - 1)
	extract = P;
	else 
	extract = np - 1;




	for (n = 0; n < extract; n++) {
		offset = portion * (n - 1);
		
		for (i = 0; i < N; i++) {
			for (j = 0; j < portion; j++) {
		part_p[i * portion + j] = mat_b[i * P + (portion * n + j)];
			
			}
		
		}
	MPI_Send(&mess_s, 1, MPI_INT, n + 1, tag3, MPI_COMM_WORLD);
	MPI_Send(part_p, portion * N, MPI_INT, n + 1, tag2, MPI_COMM_WORLD);
	
	

	}

//traitement du rest au master
int index;
if (rest != 0) {

index = (np- 1) * portion;

for (i = 0; i < M; i++) {
		for (j = index; j < index + rest; j++) {
			mpic[i][j] = 0;
			for (k = 0; k < N; k++) {
			mpic[i][j] +=  ma[i][k] * mb[k][j];

	
			}
		
		}
	
	}

}

	//Attender les resultats partiel ...

	//allocation d'un buffer<
	int * part_r = (int *)malloc(sizeof(int) * portion * M);


	//Collection de restultats de resultats

	

if (P <= np - 1) { //attente d'un sous ensemble de processus
 	for (n = 1; n <= P; n++) {
MPI_Recv(part_r, M * portion, MPI_INT, n, tag1, MPI_COMM_WORLD, &stat);		
		
	//Placement de resultat dans la matrice mpic.
		for (i = 0; i < M; i++) {
			for (j = 0; j < portion; j++) {
		mpic [i][(n - 1) * portion + j] = part_r[i * portion + j];
			
			}
		}

	}
	
//afficher le resultat

	printf("Multiplicagtion parallele:\n");
	printm(mpic, M, P);

}
else {  //attente de tous les processus

for (n = 1; n < np; n++) {
MPI_Recv(part_r, M * portion, MPI_INT, n, tag1, MPI_COMM_WORLD, &stat);		
		
	//Placement de resultat dans la matrice C.
		for (i = 0; i < M; i++) {
			for (j = 0; j < portion; j++) {
		mpic [i][(n - 1) * portion + j] = part_r[i * portion + j];
			;
			}
		}

	}
	
//afficher le resultat

	printf("Multiplicagtion parallele:\n");
	printm(mpic, M, P);




}	
	
//slaves
} else {


if (P <= np - 1) {  //un sous ensemble de processus vont faire du traitement

	if (rank <= P) {




	MPI_Recv(ma, M * N, MPI_INT, 0, tag1, MPI_COMM_WORLD, &stat);

	MPI_Recv(&mess_s, 1, MPI_INT, 0, tag3, MPI_COMM_WORLD, &stat);
	
	int * buff = (int *)malloc(N * mess_s * sizeof(int));
	MPI_Recv(buff, mess_s*N, MPI_INT, 0, tag2, MPI_COMM_WORLD, &stat);
	
	
	int * result = (int *)malloc(M * mess_s * sizeof(int));
	
	int * mat_a = (int *)ma;
	for (i = 0; i < M; i++) {
		for (j = 0; j < mess_s; j++) {
			result[i * mess_s + j] = 0;
			for (k = 0; k < N; k++) {
	result[i * mess_s + j] +=  mat_a[i * N + k] * buff[k * mess_s + j];
	
			}
		
		}
	
	}

	


	//envoi du resltat
	MPI_Send(result, M * mess_s, MPI_INT, 0, tag1,  MPI_COMM_WORLD);

	}
}

else { //tous les processus font le travail.

MPI_Recv(ma, M * N, MPI_INT, 0, tag1, MPI_COMM_WORLD, &stat);

	MPI_Recv(&mess_s, 1, MPI_INT, 0, tag3, MPI_COMM_WORLD, &stat);
	
	int * buff = (int *)malloc(N * mess_s * sizeof(int));
	MPI_Recv(buff, mess_s*N, MPI_INT, 0, tag2, MPI_COMM_WORLD, &stat);
	
	
	int * result = (int *)malloc(M * mess_s * sizeof(int));
	
	int * mat_a = (int *)ma;
	for (i = 0; i < M; i++) {
		for (j = 0; j < mess_s; j++) {
			result[i * mess_s + j] = 0;
			for (k = 0; k < N; k++) {
	result[i * mess_s + j] +=  mat_a[i * N + k] * buff[k * mess_s + j];
	
			}
		
		}
	
	}

	



	MPI_Send(result, M * mess_s, MPI_INT, 0, tag1,  MPI_COMM_WORLD);



}






}

}
MPI_Finalize();


return 0;

}
