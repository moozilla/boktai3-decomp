#include "global.h"

void Task_Kill(void *);

void ParticlePool_Destroy(void *p)
{
    Task_Kill(p);
}
