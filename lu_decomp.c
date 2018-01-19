#include <stdio.h>
#include <stdlib.h>

#define N 4

void printm(double mat [N][N] , int size) {

int i, j;

for (i = 0; i < N; i++) {
	for (j = 0; j < N; j++)
		printf("%6.2f  ", mat[i][j]);
	printf("\n");
}


}


void initmat(double mat[N][N], int size) {


int i, j;
for (i = 0; i < N; i++) {
	for (j = 0; j < N; j++)
		mat[i][j] = rand() % 10;

}



}


void matmu(double mat1[N][N], double mat2[N][N]) {

	double mat3[N][N];
	int i, j, k;

	for (i = 0; i < N; i++) {

		for (j =0; j < N; j++) {
			mat3[i][j] = 0;
			for (k = 0 ; k < N; k++)
				mat3[i][j] += mat1[i][k] * mat2[k][j];
		}

	}
	printm(mat3, N);

}




//double(*)[3] 

int main() {

int i,j, k ;
float sum;
printf("Hello \n");

double m1[N][N], l[N][N], u[N][N];



//initmat(m, N);

//double m[N][N] = {{2, -1, -2}, {-4 ,6, 3}, {-4, -2, 8}};
double m[N][N] = {{2, 1, 2, 3}, {6, 2, 4, 8}, {1, -1, 0, 4}, {0, 1, -3, -4}};
//double m[N][N] = {{2, -1, 3}, {4 ,2, 1}, {-6, -1, 2}};
//double m[N][N] = {{4, 3, -1}, {-2 , -4, 5}, {1, 2, 6}};


		  
printm(m, N);

for (j = 0; j < N; j++) {

	for (i = 0; i < N; i++) {
	
		l[i][j] = u[i][j] = 0;
		

	}

}

/*
printm(l, N);
printf("\n");
printm(u, N);
*/


for (i = 0; i < N; i++) {
	
		l[i][i] = 1;
		

	}





for (i = 0; i < N; i++) {


	for (j = 0; j < N; j++) {
		sum = 0;	
		if (j >= i) {
			for (k = 0; k <= i - 1; k++) 
				sum += l[i][k] * u[k][j];

			u[i][j] = (m[i][j] - sum) / l[i][i];

		}
		else {

			for (k = 0; k <= j-1; k++)
				sum += l[i][k] * u[k][j];
			
			l[i][j] = (m[i][j] - sum) / u[j][j];
		}

	}
}

//printf("\n");

//printm(l, N);



printf("L:\n");

printm(l, N);


printf("U: \n");

printm(u, N);


printf("\n \n mult: \n");

matmu(l, u);


return 0;

}
