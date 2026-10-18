#define ColorPulse_PrepareTargetDash  sub_080F2700
#define ColorPulse_AlignTargetHeading sub_080F2858
#include "global.h"
struct ColorPulseTarget {
    u8 pad0[0x30];
    s16 x, y, z;
};
struct ColorPulse {
    u8 pad0[0x60];
    struct ColorPulseTarget *target;
    u8 pad64[0x424];
    u16 targetX, targetY, targetZ;
    u8 pad48E[2];
    u16 deltaX, deltaY, deltaZ;
    u8 pad496[2];
    u16 startX, startY, startZ;
    u8 pad49E[0x12];
    s8 speed;
    u8 angle;
    u8 pad4B2[2];
    u16 timer;
    u8 pad4B6[0x6b8];
    u8 rotation;
    u8 padB6F[0x15];
    s16 x, y, z;
    u8 padB8A[0x14e];
    void (*callback)(struct ColorPulse *);
    u8 padCDC[0x7e];
    s16 rotationTimer;
    u8 padD5C[0x24];
    u32 ticks;
};
void sub_080F2858(struct ColorPulse *);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void sub_080F2700(struct ColorPulse *p)
{
    p->rotationTimer += p->speed;
    if (p->rotationTimer > 255) {
        p->rotationTimer = 0;
        Sound_PlaySong(361);
    }
    p->rotation += p->speed;
    if (!(p->ticks & 7)) {
        if (++p->speed > 7)
            p->speed = 7;
    }
    if (++p->timer > 150) {
        p->angle = 32 - Math_Atan2(p->x - p->target->x, p->z - p->target->z);
        p->startX = p->x;
        p->startY = p->y;
        p->startZ = p->z;
        p->targetX = p->target->x;
        p->targetY = p->target->y + 350;
        p->targetZ = p->target->z;
        p->deltaX = p->targetX - p->x;
        p->deltaY = p->targetY - p->y;
        p->deltaZ = p->targetZ - p->z;
        p->timer = 0;
        p->callback = sub_080F2858;
    }
}
