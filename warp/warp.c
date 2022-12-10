#include <math.h>
#include <stdio.h>
#include "warp.h"
#include "mat.h"

void swap(double M1[8][8], size_t i1, size_t j1, size_t i2, size_t j2){
	double tmp = M1[i1][j1];
	M1[i1][j1] = M1[i2][j2];
	M1[i2][j2] = tmp;
}

void printMat(double M1[8][8]){
	for(size_t i = 0; i < 8; i++){
		for(size_t j = 0; j < 8; j++){
			printf(" %2.2f|", M1[i][j]);
		}
		printf("\n");
	}
}

void transposeMat(double M1[8][8], double M1_t[8][8]){
	for(size_t i = 0; i < 8; i++){
		for(size_t j = 0; j < 8; j++){
			M1_t[j][i] = M1[i][j];
		}
	}
}

void CopyMat8(double M1[8][8], double M2[8][8]){
	for(size_t i = 0; i < 8; i++){
                for(size_t j = 0; j < 8; j++){
                        M2[i][j] = M1[i][j];
                }
        }
}


void CopyMat8x1(double M1[8], double M2[8]){
	for(size_t i = 0; i < 8; i++){
		M2[i] = M1[i];
        }
}

void CopyMat3x1(double M1[3], double M2[3]){
	for(size_t i = 0; i < 3; i++)
		M2[i] = M1[i];
}

void MultMat3x1(double H[3][3], double X[3], double res[3]){
	double _res[3] = {};
	for(size_t i = 0; i < 3; i++){
		double sum = 0;
		for(size_t j = 0; j < 3; j++)
			sum += H[i][j]*X[j];
		_res[i] = sum;
	}
	CopyMat3x1(_res, res);
}

void MultMat8(double M1[8][8], double M2[8][8], double res[8][8]){
	double _res[8][8] = {};
	for(size_t i = 0; i < 8; i++){
	for(size_t j = 0; j < 8; j++){
		double sum = 0;
		for(size_t k = 0; k < 8; k++){
			sum += M1[i][k]*M2[k][j];
		}
		_res[i][j] = sum;
	}
	}
	CopyMat8(_res, res);
}


void MultMat8x1(double M1[8][8], double M2[8], double res[8]){
	double _res[8] = {};
	for(size_t i = 0; i < 8; i++){
	double sum = 0;
	for(size_t j = 0; j < 8; j++){
		sum += M1[i][j]*M2[j];
	}
	_res[i] = sum;
	}
	CopyMat8x1(_res, res);
}

void computeH(struct Rectangle A, struct Rectangle B, double H[3][3]){
	double M1[8][8] = {};
	for(size_t i = 0; i < 4; i++){
		M1[2*i][0] = A.px[i];
		M1[2*i][1] = A.py[i];
		M1[2*i][2] = 1;
		M1[2*i][3] = 0;
		M1[2*i][4] = 0;
		M1[2*i][5] = 0;
		M1[2*i][6] = -A.px[i]*B.px[i];
		M1[2*i][7] = -A.py[i]*B.px[i];

		M1[2*i+1][0] = 0;
		M1[2*i+1][1] = 0;
		M1[2*i+1][2] = 0;
		M1[2*i+1][3] = A.px[i];
		M1[2*i+1][4] = A.py[i];
		M1[2*i+1][5] = 1;
		M1[2*i+1][6] = -A.px[i]*B.py[i];
		M1[2*i+1][7] = -A.py[i]*B.py[i];
	}

	double M2[8] = {};
	for(size_t i = 0; i < 4; i++){
		M2[2*i] = B.px[i];
		M2[2*i+1] = B.py[i];
	}

	double M1_t[8][8] = {};
	transposeMat(M1, M1_t);

	//H = (M1_t * M1)**-1 * (M1_t * M2)
	
	MultMat8(M1_t, M1, M1);
	double inv[8][8] = {};
	if(determinant(M1, 8) == 0)
		printf("Not Inversible !\n");
	else
		cofactor(M1, 8, inv);
	//printf("Inversed\n");
	//printMat(inv);
	MultMat8x1(M1_t, M2, M2);
	MultMat8x1(inv, M2, M2);
	
	H[2][2] = 1;
	printf("H =\n");
	for(size_t i = 0; i < 8; i++){
		H[i/3][i%3] = M2[i];
	}
	for(size_t i = 0; i < 3; i++){
		for(size_t j = 0; j < 3; j++)
			printf(" %2.2f |", H[i][j]);
		printf("\n");
	}
	printf("\n");
}
