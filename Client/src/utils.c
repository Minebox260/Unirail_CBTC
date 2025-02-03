#include <math.h>
#include "utils.h"
#include <stdio.h>

// Convert Marvelmind accelerometer data to normed value
double calculateAccNnorm(int16_t aX, int16_t aY, int16_t aZ) {
	double norm = sqrt((double)aX * aX + (double)aY * aY + (double)aZ * aZ);
	printf("X:%d\tY:%d\tZ:%d\t\t%f\t", aX, aY, aZ, norm);
	return MARVELMIND_ACC_MG_TO_MS2_RATIO * MARVELMIND_ACC_LSB_SCALE_FACTOR * norm; 
}