#include "global.h"
#include "libm_compat.h"

double sub_08229560(double arg)
{
    double base, result;
    base = (((arg - 51544.5) + 0.00074074) / 36525.0);
    result = (23.4393 - (base * 0.013));
    return (result + (cos((((base * 1934.0) + 235.0) * 0.017453292519943278)) * 0.0026));
}
