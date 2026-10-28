#include "global.h"
#include "engine_globals.h"
#include "ram.h"
// Provisional name: Presentation_UpdateDropShakeBlink
// MATCHING_CONCESSION: register-pinning
struct Sequence {
    u8 p0[0x18];
    u8 resource[0x20];
    u8 layer0[8];
    u32 flags0;
    u8 p1[0x98 - 0x44];
    u8 layer1[8];
    u32 flags1;
    u8 p2[0xb8 - 0xa4];
    s16 x1, y1;
    u8 p3[0xf8 - 0xbc];
    u8 layer2[8];
    u32 flags2;
    u8 p4[0x158 - 0x104];
    u8 layer3[8];
    u32 flags3;
    u8 p5[0x178 - 0x164];
    s16 x3, y3;
    u8 p6[0x1b8 - 0x17c];
    u8 layer4[8];
    u32 flags4;
    u8 p7[0x21c - 0x1c4];
    u16 option, state, timer;
    u8 p9[2];
    u16 frame, last, frameTimer, duration, result, exitTimer;
    void *handle;
    void (*callback)(void *);
};
void Anim_LoadKeyframe(void *, void *, s32, s32);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
void Presentation_UpdateLayerAnimations(void *);
void Presentation_ClearTilemap(void);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline s32 random(u16 *input)
{
    register u16 *table asm("r6") = input;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
// Animate a presentation layer dropping, shaking, then blinking with its alternate.
void Presentation_UpdateDropShakeBlink(struct Sequence *p)
{
    if (p->state < 30) {
        p->flags1 &= ~1;
        p->x1 = 60;
        p->y1 = -30;
        p->flags3 |= 1;
    } else if (p->state < 45) {
        p->flags1 &= ~1;
        p->x1 = 60;
        {
            s32 y = p->state * 2 - 90;
            register s16 *dest asm("r3") = &p->y1;
            *dest = y;
        }
        p->flags3 |= 1;
    } else if (p->state < 50) {
        p->x1 = Mod(random(gRandomTable), 5) + 58;
        {
            s32 y = (45 - p->state) * 2 + Mod(random(gRandomTable), 5) - 2;
            register s16 *dest asm("r3") = &p->y1;
            *dest = y;
        }
    } else if (p->state < 55) {
        p->x1 = Mod(random(gRandomTable), 5) + 58;
        {
            s32 y = (p->state - 55) * 2 + Mod(random(gRandomTable), 5) - 2;
            register s16 *dest asm("r3") = &p->y1;
            *dest = y;
        }
    } else if (p->state < 60) {
        p->x1 = Mod(random(gRandomTable), 5) + 58;
        p->y1 = Mod(random(gRandomTable), 5) - 2;
        p->flags3 |= 1;
    } else if (p->state < 80) {
        p->x1 = Mod(random(gRandomTable), 3) + 59;
        p->y1 = Mod(random(gRandomTable), 3) - 1;
        p->flags3 |= 1;
    } else if (p->state < 90) {
        p->x1 = 60;
        p->y1 = 0;
    } else if (Div(p->state, 45) & 1) {
        p->flags1 |= 1;
        p->flags3 &= ~1;
    } else {
        p->flags1 &= ~1;
        p->flags3 |= 1;
    }
    p->state++;
}
