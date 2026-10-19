
#ifdef NONMATCHING
// fuzzy: 96.9%
// foundry draft (worker best 96.9%); what differs is mostly register allocation and instruction order
#define ColorPulse_AlignTargetHeading sub_080F2858
#define ColorPulse_BeginNearApproach  sub_080F2960
#define ColorPulse_BeginFarApproach   sub_080F2AAC
#include "global.h"
struct ColorPulsePart {
    u32 flags;
    u8 pad4[0x28];
};
struct ColorPulseTarget {
    u8 pad0[0x30];
    s16 x, y, z;
};
struct ColorPulse {
    u8 pad0[0x60];
    struct ColorPulseTarget *target;
    u8 pad64[0x44];
    struct ColorPulsePart parts[4];
    u8 pad158[0x330];
    u16 targetX, targetY, targetZ;
    u8 pad48E[2];
    s16 deltaX, deltaY, deltaZ;
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
void sub_080F2960(struct ColorPulse *);
void sub_080F2AAC(struct ColorPulse *);
void Sound_PlaySong(u32);
void sub_080F2858(struct ColorPulse *p)
{
    s32 i;
    p->rotationTimer += p->speed;
    if (p->rotationTimer > 255) {
        p->rotationTimer = 0;
        Sound_PlaySong(361);
    }
    i = 0;
    if (i < p->speed) {
        u8 *rotation = &p->rotation;
        u32 zero = 0;
        u16 *timer = &p->timer;
        s8 *speed = &p->speed;
        void (**state)(struct ColorPulse *) = &p->callback;
        u8 *angle = &p->angle;
        s16 *dx = &p->deltaX;
        s16 *dz = &p->deltaZ;
        struct ColorPulsePart *volatile parts;
        p = (struct ColorPulse *)((u8 *)p + 0xa8);
        parts = (struct ColorPulsePart *)p;
        do {
            if (++*rotation == *angle) {
                s32 x, z;

                *rotation = *angle;
                *timer = zero;
                *speed = zero;
                x = *dx * *dx;
                z = *dz * *dz;
                if (x + z < 1000000)
                    *state = sub_080F2960;
                else {
                    void (*callback)(struct ColorPulse *) = sub_080F2AAC;
                    u16 one = 1;
                    struct ColorPulsePart *part = parts;
                    struct ColorPulsePart *end = part + 3;
                    do {
                        part->flags |= one;
                        part++;
                    } while ((s32)part <= (s32)end);
                    *state = callback;
                }
                return;
            }
            i++;
        } while (i < *speed);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080F2858);
#endif
