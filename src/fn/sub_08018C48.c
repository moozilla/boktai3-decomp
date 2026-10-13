#include "global.h"
void *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
int sub_08018898(void *);
int sub_080189A8(void *);
int sub_080189C8(void *, void *, int, int, int, int);
void sub_0821A0C0(void *);
#define OrbitingParticles_Create sub_08018C48
void *OrbitingParticles_Create(void *pos, int radius, int duration, int count, int type)
{
    void *p = sub_08219FBC(11, 436);
    if (p) {
        sub_0821A04C(p, sub_08018898, sub_080189A8);
        if (sub_080189C8(p, pos, radius, duration, count, type) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
