#include "global.h"
#include "libm_compat.h"
double sub_08229468(s32, s32, s32, s32);
double sub_08229618(double);
double sub_08229E28(double);

double sub_08229FFC(s32 a, s32 year, s32 month, s32 day)
{
    double start, current, difference, age;
    double rate = 12.1818;
    double period = 360.0 / 12.1818;
    s32 i;
    current = sub_08229468(a,year,month,day) + 0.5;
    start = current;
    for (i=1; i<=6; i++) {
        difference = sub_08229618(current) - sub_08229E28(current);
        if (difference >= 0.0 && difference <= 0.1) break;
        if (difference < 0.0 && difference >= -0.1) break;
        current -= difference / rate;
    }
    age = start - current;
    if (age < 0.0 || age > period) {
        if (age < 0.0) {
            current -= period;
            if (age < -period) current -= period;
        } else current += period;
        for (i=1; i<=4; i++) {
            difference = sub_08229618(current) - sub_08229E28(current);
            if (difference >= 0.0 && difference <= 0.1) break;
            if (difference < 0.0 && difference >= -0.1) break;
            current -= difference / rate;
        }
    }
    age = start - current;
    return age;
}
