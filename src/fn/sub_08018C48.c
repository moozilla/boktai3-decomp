#include "global.h"
void *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
int OrbitingParticles_Update(void *);
int sub_080189A8(void *);
int OrbitingParticles_Initialize(void *, void *, int, int, int, int);
void sub_0821A0C0(void *);
void *OrbitingParticles_Create(void *pos, int radius, int duration, int count, int type)
{
    void *p = sub_08219FBC(11, 436);
    if (p) {
        sub_0821A04C(p, OrbitingParticles_Update, sub_080189A8);
        if (OrbitingParticles_Initialize(p, pos, radius, duration, count, type) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
