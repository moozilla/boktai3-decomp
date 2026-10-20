#define ActorDisplay_UpdateForegroundSprite sub_081EC7D8
#define ActorDisplay_UpdateAccentSprite     sub_081EC974
// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: role inferred from state transitions and callees.
#define SceneActors_UpdatePaletteDispersal sub_081EEAD8
#ifdef NONMATCHING
// fuzzy: 89.4%
// Remaining differences: initialization allocation and palette/loop operand ordering.
struct P {
    u8 unknown[0x630];
};
struct PackedVec {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
extern u8 *gUnk_030042E4;
extern u16 gUnk_03005040[4][16];
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
void Palette_InterpolateBanks(void *, const void *, const void *, s32, s32);
void sub_08113398(struct PackedVec *, struct PackedVec *, void *, void *, s32, s32, s32, s32, s32,
                  s32, s32, s32, s32, s32);
void ActorDisplay_UpdateScreenAnchor(struct P *);
void ActorDisplay_UpdateForegroundSprite(struct P *);
void ActorDisplay_UpdateAccentSprite(struct P *);
#define B(o)   (*(u8 *)((u8 *)p + (o)))
#define W(o)   (*(s32 *)((u8 *)p + (o)))
#define PTR(o) (*(void **)((u8 *)p + (o)))
static inline u32 Random(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
}
// confidence=low: fades two palettes then emits dispersal particles.
void SceneActors_UpdatePaletteDispersal(struct P *arg)
{
    struct P *input = arg;
    struct P *p = input;
    if ((*(u8 *)((u8 *)input + 0xb1))) {
        (*(u8 *)((u8 *)input + 0xb1)) = 0;
        (*(s32 *)((u8 *)input + 0x394)) |= 0x20000;
        (*(s32 *)((u8 *)input + 0x5a8)) = 0;
        (*(s32 *)((u8 *)input + 0x5ac)) = 6;
        (*(s32 *)((u8 *)input + 0x398)) = 0;
        (*(s32 *)((u8 *)input + 0x5a0)) = 0;
    }
    switch (W(0x398)) {
        case 0: {
            register s32 i asm("r8");
            register u8 *src asm("r4");
            register u8 *dest asm("r5");
            W(0x5a8)++;
            Palette_InterpolateBanks((u8 *)p + 0x3c8, gUnk_030042E4 + 0x60, gUnk_030042E4 + 0x3ec0,
                                     W(0x5a8), W(0x5ac));
            PTR(0x35c) = (u8 *)p + 0x3c8;
            Palette_InterpolateBanks((u8 *)p + 0x3e8, gUnk_030042E4 + 0x40, gUnk_030042E4 + 0x3ea0,
                                     W(0x5a8), W(0x5ac));
            PTR(0x378) = (u8 *)p + 0x3e8;
            i = 0;
            dest = (u8 *)p + 0x488;
            src = (u8 *)p + 0x408;
            do {
                Palette_InterpolateBanks(gUnk_03005040[i], src, dest, W(0x5a8), W(0x5ac));
                dest += 32;
                src += 32;
            } while (++i <= 3);
            if (W(0x5a8) > (1 << W(0x5ac))) {
                W(0x304) = 1;
                W(0x34c) = 1;
                W(0x398) = 1;
            } else {
                W(0x304) = 0;
                W(0x34c) = 0;
                if (W(0x5a8) > (1 << (W(0x5ac) - 1))) {
                    if (W(0x5a8) & 1) {
                        W(0x304) = 1;
                        W(0x34c) = 1;
                    }
                    if (W(0x5a8) > (1 << (W(0x5ac) - 2)) && Mod(W(0x5a8), 3) == 0) {
                        W(0x304) = 1;
                        W(0x34c) = 1;
                    }
                }
            }
            break;
        }
        case 1:
            if (++W(0x5a0) > 49)
                W(0x398) = 2;
            break;
        case 2: {
            struct PackedVec pos, vel;
            register s32 i asm("r8");
            i = 4;
            do {
                pos.x = (W(0x268) >> 12) + 50;
                pos.y = 138;
                pos.z = 50;
                vel.x = (Random() & 3) - 1;
                vel.y = -(Random() & 1);
                vel.z = 0;
                sub_08113398(&pos, &vel, PTR(0x388), PTR(0x38c), 5, 0, 0, 0, 2, 42, 0, 512, 1, 0);
                W(0x59c)++;
            } while (--i >= 0);
            if (W(0x59c) > 23)
                W(0x398) = 3;
            break;
        }
        case 3:
            B(0xaf) = 1;
            break;
    }
    ActorDisplay_UpdateScreenAnchor(input);
    ActorDisplay_UpdateForegroundSprite(input);
    ActorDisplay_UpdateAccentSprite(input);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081EEAD8);
#endif
