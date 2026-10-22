#include "global.h"
#define Sword_DampTargetReturn sub_080F5C9C
struct Motion0818Object {
    u8 pad0[6];
    u8 rotation; /* +006: wrapping orientation byte */
    u8 pad7[0x15];
    s16 x, y, z; /* +01c: current position */
    u8 pad22[0x30];
    u16 field52;
    u8 pad54[0x52];
    u16 fieldA6;
    u8 padA8[0x58];
    s16 velocityX; /* +100: per-update X displacement */
    u16 pad102;
    s16 velocityZ; /* +104: per-update Z displacement */
    u8 pad106[0x13];
    u8 flags119, angle, pad11B[3], byte11E, byte11F;
    u8 pad120[0x14];
    u16 endX, endY, endZ, pad13A;       /* +134: endpoint */
    u16 startX, startY, startZ, pad142; /* +13c: segment origin */
    s16 dx, dy, dz;
    u16 pad14A;
    u16 fx, fy, fz, pad152; /* +14c: initial owner-relative offset */
    u16 a, b, c, pad15A;    /* +154: phase-dependent scratch vector */
    u16 *target;
    u8 *parent;             /* +15c: owner; position at +5ac */
    u8 pad164[0x18];
    void (*callback)(struct Motion0818Object *);
    u32 unknown180;
    s32 timer;
    u8 targetRotation, pad189;
    s8 turnDirection;
    u8 done, pad18C;
    s8 byte1DD;
};

struct ReturnDampingCounter {
    u8 pad[0x126];
    u16 ticks;
};
s32 Mod(s32, s32);
void TargetMotion_AdvanceFollowSegment(struct Motion0818Object *);
void Sword_DampTargetReturn(struct Motion0818Object *p)
{
    s32 value, half;
    p->rotation += 20;
    p->x += p->velocityX;
    p->z += p->velocityZ;
    if (Mod(++((struct ReturnDampingCounter *)p)->ticks, 5) == 0) {
        if (p->velocityX == 0 && p->velocityZ == 0) {
            p->fieldA6 = 16;
            p->field52 = 16;
            p->timer = 0;
            p->a = p->target[0] - p->fx - p->startX;
            p->b = p->target[1] - p->fy - p->startY;
            p->c = p->target[2] - p->fz - p->startZ;
            p->endX = p->startX + p->a;
            p->endY = p->startY + p->b;
            p->endZ = p->startZ + p->c;
            p->startX = p->x;
            p->startY = p->y;
            p->startZ = p->z;
            p->dx = p->endX - p->startX;
            p->dy = p->endY - p->startY;
            p->dz = p->endZ - p->startZ;
            p->a = p->target[0];
            p->b = p->target[1];
            p->c = p->target[2];
            p->turnDirection = -p->turnDirection;
            p->byte1DD = -15;
            p->callback = TargetMotion_AdvanceFollowSegment;
        }
        value = p->velocityX;
        if (value >= 0)
            half = value >> 1;
        else
            half = -(-value >> 1);
        p->velocityX = half;
        value = p->velocityZ;
        if (value >= 0)
            half = value >> 1;
        else
            half = -(-value >> 1);
        p->velocityZ = half;
    }
}
