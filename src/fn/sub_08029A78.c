#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
void GridAttackActor_GetGridPosition(void *, int, int);
void ContactSequencePool_SpawnActor(void *, void *, int);
void GridAttackActor_SpawnGridContact(u8 *p, int x, int z)
{
    struct Vec pos;
    GridAttackActor_GetGridPosition(&pos, x, z);
    ContactSequencePool_SpawnActor(*(void **)(p + 0x904), &pos, x + z * 3);
}
