#include <stdlib.h>
#include <math.h>
#include "float24.h"

float float24_asIEEE(Float24_t x)
{
    return (float)x.mantissa * powf(2, x.exponent);
}

