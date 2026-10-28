#include "global.h"
#include "engine_globals.h"

// confidence=low: steps the four entry weights and, on a timer, re-aims the pulse at the target.

struct ColorPulsePart {
    u32 flags;
    u8 pad4[2];
    u8 mode;
    u8 pad7;
    s8 weight;
    u8 pad9[0x13];
    s16 x, y, z;
    u8 pad22[0xA];
};

// confidence=low: steps the four entry weights and, on a timer, re-aims the pulse at the target.

// confidence=low: steps the four entry weights and, on a timer, re-aims the pulse at the target.
struct ColorPulseTarget {
    u8 pad0[0x30];
    s16 x, y, z;
};
struct ColorPulsePoint {
    s16 x, y, z, pad;
};
struct ColorPulse {
    u8 pad0[0x60];
    struct ColorPulseTarget *target;
    s16 posA;
    u8 pad66[2];
    s16 posB;
    u8 pad6A[2];
    u16 counterA;
    u8 pad6E[2];
    u16 counterB;
    u8 pad72[0x468 - 0x72];
    struct ColorPulsePoint saved[4];
    struct ColorPulsePoint goal;
    u8 pad490[0xB6E - 0x490];
    u8 phase;
    u8 padB6F[0xB84 - 0xB6F];
    struct ColorPulsePoint origin;
    u8 padB8C[0xCE0 - 0xB8C];
    void (*callbackCE0)(struct ColorPulse *);
    u8 padCE4[0xCF4 - 0xCE4];
    s32 delay;
    u8 padCF8[0xD80 - 0xCF8];
    u32 flagsD80;
};
extern u16 gUnk_0203B400[];
extern s16 gUnk_086149C4[];
s32 Div(s32, s32);
void PhaseObject_BeginEntryMotion(struct ColorPulse *);

#ifdef NONMATCHING
// fuzzy: 63.3%
// scope shape 74 (differences once low registers are set aside)
// confidence=low: steps the four entry weights and, on a timer, re-aims the pulse at the target.
void sub_080F2F7C(struct ColorPulse *p)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        u8 *e = (u8 *)p + i * 0x2c;
        p->posA = -p->posA;
        p->posB = -p->posB;
        *(s16 *)(e + 0xC4) += p->posA;
        *(s16 *)(e + 0xC8) -= p->posB;
        *(s8 *)(e + 0xB0) -= 2;
        if (*(s8 *)(e + 0xB0) <= 1)
            *(s8 *)(e + 0xB0) = 1;
    }
    if (!(p->flagsD80 & 1)) {
        p->counterA++;
        p->counterB++;
        p->posA = p->counterA;
        p->posB = p->counterB;
        if ((s16)p->counterA > 15) {
            u32 sel;
            struct ColorPulsePoint *sv = (struct ColorPulsePoint *)p;

            p->posA = *(s16 *)((u8 *)p + 0xC4) - p->origin.x;
            p->posB = *(s16 *)((u8 *)p + 0xC8) - p->origin.z;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            sel = *(u8 *)&gUnk_0203B400[gRandomIndex];
            p->origin.x = p->target->x + Div(gUnk_086149C4[(sel + 64) & 255] * 200, 4096);
            p->origin.y = p->target->y + 350;
            p->origin.z = p->target->z + Div(gUnk_086149C4[sel & 255] * 200, 4096);
            for (i = 0; i < 4; i++) {
                u8 *e = (u8 *)p + i * 0x2c;
                s16 v;

                *(u32 *)(e + 0xA8) |= 1;
                v = p->posA + p->origin.x;
                sv[0x8d].x = v;
                *(s16 *)(e + 0xC4) = v;
                v = p->posB + p->origin.z;
                sv[0x8d].z = v;
                *(s16 *)(e + 0xC8) = v;
                p->goal.x = p->origin.x;
                p->goal.y = p->origin.y;
                p->goal.z = p->origin.z;
                p->posA = -p->posA;
                p->posB = -p->posB;
                *(u8 *)(e + 0xAE) = 0x80;
                sv++;
            }
            p->phase = 0x80;
            p->delay = 30;
            p->callbackCE0 = PhaseObject_BeginEntryMotion;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080F2F7C);
#endif
