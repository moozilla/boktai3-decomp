#include "global.h"
#include "libm_compat.h"

double sub_08229E28(double arg)
{
    double base, result, t;
    base = (((arg - 51544.5) + 0.00074074) / 365.25);
    result = ((base * 360.00769) + 280.4603);
    t = (1.9146 - (base * 5e-05));
    result = (result + (t * sin((((base * 359.991) + 357.538) * 0.017453292519943278))));
    result = (result + (sin((((base * 719.981) + 355.05) * 0.017453292519943278)) * 0.02));
    result = (result + (sin((((base * 19.341) + 234.95) * 0.017453292519943278)) * 0.0048));
    result = fmod(result, 360.0);
    if (result < 0.0) result += 360.0;
    return result;
}
