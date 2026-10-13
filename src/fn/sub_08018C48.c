#include "global.h"
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int OrbitingParticles_Update(void *);
int sub_080189A8(void *);
int OrbitingParticles_Initialize(void *, void *, int, int, int, int);
void Task_Kill(void *);
void *OrbitingParticles_Create(void *pos, int radius, int duration, int count, int type)
{
    void *p = Task_Create(11, 436);
    if (p) {
        Task_SetCallbacks(p, OrbitingParticles_Update, sub_080189A8);
        if (OrbitingParticles_Initialize(p, pos, radius, duration, count, type) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
