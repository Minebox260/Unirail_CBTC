#include <stdint.h>
#define MARVELMIND_ACC_LSB_SCALE_FACTOR 1
#define MARVELMIND_ACC_MG_TO_MS2_RATIO 0.00980665

double calculateAccNnorm(int16_t aX, int16_t aY, int16_t aZ);