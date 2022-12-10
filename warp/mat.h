#ifndef MAT_H
#define MAT_H
void cofactor(double num[8][8], double f, double res[8][8]);
void transpose(double num[8][8], double fac[8][8], double r, double res[8][8]);
double determinant(double a[8][8], double k);
void MultMat3x1(double H[3][3], double X[3], double res[3]);
#endif
