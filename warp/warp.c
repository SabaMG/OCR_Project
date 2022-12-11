#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "warp.h"

/*For calculating Determinant of the Matrix */
double determinant(double a[8][8], double k)
{
  double s = 1, det = 0, b[8][8];
  int i, j, m, n, c;
  if (k == 1)
    {
     return (a[0][0]);
    }
  else
    {
     det = 0;
     for (c = 0; c < k; c++)
       {
        m = 0;
        n = 0;
        for (i = 0;i < k; i++)
          {
            for (j = 0 ;j < k; j++)
              {
                b[i][j] = 0;
                if (i != 0 && j != c)
                 {
                   b[m][n] = a[i][j];
                   if (n < (k - 2))
                    n++;
                   else
                    {
                     n = 0;
                     m++;
                     }
                   }
               }
             }
          det = det + s * (a[0][c] * determinant(b, k - 1));
          s = -1 * s;
          }
    }
 
    return (det);
}

/*Finding transpose of matrix*/ 
void transpose(double num[8][8], double fac[8][8], double r, double res[8][8])
{
  int i, j;
  double b[8][8], inverse[8][8], d;
 
  for (i = 0;i < r; i++)
    {
     for (j = 0;j < r; j++)
       {
         b[i][j] = fac[j][i];
        }
    }
  d = determinant(num, r);
  for (i = 0;i < r; i++)
    {
     for (j = 0;j < r; j++)
       {
        inverse[i][j] = b[i][j] / d;
        }
    }
   //printf("\n\n\nThe inverse of matrix is : \n");
 
   for (i = 0;i < r; i++)
    {
     for (j = 0;j < r; j++)
       {
         //printf("\t%lf", inverse[i][j]);
	 res[i][j] = inverse[i][j];
        }
    //printf("\n");
     }
}

void cofactor(double num[8][8], double f, double res[8][8])
{
 double b[8][8], fac[8][8];
 int p, q, m, n, i, j;
 for (q = 0;q < f; q++)
 {
   for (p = 0;p < f; p++)
    {
     m = 0;
     n = 0;
     for (i = 0;i < f; i++)
     {
       for (j = 0;j < f; j++)
        {
          if (i != q && j != p)
          {
            b[m][n] = num[i][j];
            if (n < (f - 2))
             n++;
            else
             {
               n = 0;
               m++;
               }
            }
        }
      }
      fac[q][p] = pow(-1, q + p) * determinant(b, f - 1);
    }
  }
  transpose(num, fac, f, res);
}

///Warp part

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

void computeH(struct Rectangle* A, struct Rectangle* B, double H[3][3]){
	double M1[8][8] = {};
	for(size_t i = 0; i < 4; i++){
		M1[2*i][0] = A->px[i];
		M1[2*i][1] = A->py[i];
		M1[2*i][2] = 1;
		M1[2*i][3] = 0;
		M1[2*i][4] = 0;
		M1[2*i][5] = 0;
		M1[2*i][6] = -A->px[i]*B->px[i];
		M1[2*i][7] = -A->py[i]*B->px[i];

		M1[2*i+1][0] = 0;
		M1[2*i+1][1] = 0;
		M1[2*i+1][2] = 0;
		M1[2*i+1][3] = A->px[i];
		M1[2*i+1][4] = A->py[i];
		M1[2*i+1][5] = 1;
		M1[2*i+1][6] = -A->px[i]*B->py[i];
		M1[2*i+1][7] = -A->py[i]*B->py[i];
	}

	double M2[8] = {};
	for(size_t i = 0; i < 4; i++){
		M2[2*i] = B->px[i];
		M2[2*i+1] = B->py[i];
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

void Warp(SDL_Surface* imgOrigin, SDL_Surface* imgWarped,
 struct Rectangle* src, struct Rectangle* dest){

	Uint32* pixels_src = imgOrigin->pixels;
	Uint32* pixels_dest = imgWarped->pixels;

	//Compute Homographic matrix
	double H[3][3] = {};
	computeH(src, dest, H);

	for(int x = 0; x < imgOrigin->w; x++){
	for(int y = 0; y < imgOrigin->h; y++){
		
		double X[3] = {x, y, 1};
		double tmp[3] = {};
		MultMat3x1(H, X, tmp);
		int x2 = (int)(tmp[0]/tmp[2]);
		int y2 = (int)(tmp[1]/tmp[2]);
		//printf("%lf, %lf, %lf\n", tmp[0], tmp[1], tmp[2]);
		//printf("%i,%i --> %i,%i\n", x, y, x2, y2);
		if(((x2>=0)&&(x2<imgWarped->w))&&(y2>=0)&&(y2<imgWarped->h)){
			//printf("%i,%i --> %i,%i\n", x, y, x2, y2);
			Uint32 pixel = pixels_src[y * imgOrigin->w + x];
			Uint8 r = pixel >> 16 & 0xFF;
                    	Uint8 g = pixel >> 8 & 0xFF;
                    	Uint8 b = pixel & 0xFF;
			pixels_dest[y2*imgWarped->w+x2] = (0xff<<24)|(r<<16)|(g<<8)|(b);
		}
	}
	}
	IMG_SaveJPG(imgWarped, "warped.jpg", 100);
}
