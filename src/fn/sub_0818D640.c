#define Armor_DampPartMovement sub_0818D640
#include "global.h"
#include "motion_0818.h"

void Armor_UpdatePartRetraction(struct Motion0818Object *);

void sub_0818D640(struct Motion0818Object *p)
{
    u8 *parent = p->parent;
    p->rotation += 20;
    p->x += p->velocityX;
    p->z += p->velocityZ;
    if (!(parent[0x5C2] & 7)) {
        s32 value, half;
        if (p->velocityX == 0 && p->velocityZ == 0) {
            p->fieldA6 = 16;
            p->field52 = 16;
            p->endX = p->x;
            p->endY = p->y;
            p->endZ = p->z;
            p->dx = p->endX - p->startX;
            p->dy = p->endY - p->startY;
            p->dz = p->endZ - p->startZ;
            p->callback = Armor_UpdatePartRetraction;
        }
        value = p->velocityX;
        if (value >= 0) half = value >> 1;
        else half = -(-value >> 1);
        p->velocityX = half;
        value = p->velocityZ;
        if (value >= 0) half = value >> 1;
        else half = -(-value >> 1);
        p->velocityZ = half;
    }
}
