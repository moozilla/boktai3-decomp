#include "global.h"

void CollisionVolume_InitCollider(void *);
void sub_08163C84(void *);

s32 ParticlePool_Init(void *p)
{
    CollisionVolume_InitCollider(p);
    sub_08163C84(p);
    return 0;
}
