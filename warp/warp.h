#ifndef WARP_H
#define WARP_H

struct Rectangle{
        double px[4];
        double py[4];
};
void swap(double M1[8][8], size_t i1, size_t j1, size_t i2, size_t j2);
void printMat(double M1[8][8]);
void transposeMat(double M1[8][8], double M1_t[8][8]);
void computeH(struct Rectangle A, struct Rectangle B, double H[3][3]);
void MultMat8(double M1[8][8], double M2[8][8], double res[8][8]);
void MultMat8x1(double M1[8][8], double M2[8], double res[8]);

#endif
