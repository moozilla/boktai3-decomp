#include "global.h"
#include "lib/libm/libm_compat.h"
#include "system/clock/solar_time_context.h"
double Time_CalculateSolarLongitude(double);
double Time_CalculateSolarLatitude(double);
double Time_CalculateEclipticObliquity(double);
void Time_CalculateSolarRiseTransitSet(struct SolarTimeContext *s, s32 mode, double arg, s32 day,
                                       double longitude, double latitude)
{
    double l, b, e, decl, alpha, hour;
    double x, y;
    double lat;
    double rise, date;

    l = Time_CalculateSolarLongitude(arg);
    b = Time_CalculateSolarLatitude(arg);
    e = Time_CalculateEclipticObliquity(arg);
    b /= 57.29577951308238;
    l /= 57.29577951308238;
    e /= 57.29577951308238;
    decl = asin(cos(b) * sin(l) * sin(e) + sin(b) * cos(e));
    x = cos(b) * cos(l);
    y = cos(b) * sin(l) * cos(e) - sin(b) * sin(e);
    alpha = atan(y / x);
    if (x < 0.0)
        alpha += 3.14159265358979;
    if (alpha < 0.0)
        alpha += 6.28318530717958;
    lat = latitude * 0.017453292519943278;
    hour = acos(-tan(lat) * tan(decl) + cos(1.5856316254368468) / (cos(lat) * cos(decl)));
    rise = (alpha - hour) / 6.28318530717958;
    s->d[3] = rise;
    {
        double transit;
        double value;
        double offset;
        transit = alpha / 6.28318530717958;
        s->d[4] = transit;
        s->d[5] = (alpha + hour) / 6.28318530717958;
        date = day;
        if (mode == 1) {
            if (date + rise < s->d[9] && s->n[1] == 0) {
                date += 1.0;
                s->n[0]++;
            } else if (date + s->d[3] > s->d[10] && s->n[0] == 0) {
                date -= 1.0;
                s->n[1]++;
            }
        } else if (mode == 2) {
            double t = date + transit;
            if (t < s->d[9])
                date += 1.0;
            else if (t > s->d[10])
                date -= 1.0;
        } else if (mode == 3) {
            if (s->d[5] > 1.0)
                s->d[5] -= 1.0;
            if (date + s->d[5] < s->d[9] && s->n[3] == 0) {
                date += 1.0;
                s->n[2]++;
            } else if (date + s->d[5] > s->d[10] && s->n[2] == 0) {
                date -= 1.0;
                s->n[3]++;
            }
        }
        value = (date + s->d[3]) - 0.671262;
        offset = longitude / 360.0;
        s->d[0] = (value - offset) / 1.002737909 + 40000.0;
        s->d[1] = ((date + s->d[4] - 0.671262) - offset) / 1.002737909 + 40000.0;
        s->d[2] = ((date + s->d[5] - 0.671262) - offset) / 1.002737909 + 40000.0;
    }
}
