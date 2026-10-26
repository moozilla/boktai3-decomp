// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
// confidence=low: fades sprite palettes before emitting dispersal particles.
#ifdef NONMATCHING
// fuzzy: 94.6%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: palette destination setup and particle-loop initialization order.
struct P {
    u8 unknown[0x630];
};
struct PackedVec {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};
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
    typeof(-(Random() & 1)) t1;
    if (*((u8 *)(((u8 *)input) + 0xb1))) {
        register s32 zero asm("r3") = 0;
        *((u8 *)(((u8 *)input) + 0xb1)) = zero;
        asm("" : "+r"(zero));
        *((s32 *)(((u8 *)input) + 0x394)) |= 0x20000;
        *((s32 *)(((u8 *)input) + 0x5a8)) = zero;
        *((s32 *)(((u8 *)input) + 0x5ac)) = 6;
        *((s32 *)(((u8 *)input) + 0x398)) = zero;
        *((s32 *)(((u8 *)input) + 0x5a0)) = zero;
    }
    switch (*((s32 *)(((u8 *)p) + 0x398))) {
        case 0: {
            register s32 i asm("r8");
            register u8 *src asm("r4");
            register u8 *dest asm("r5");
            (*((s32 *)(((u8 *)p) + 0x5a8)))++;
            Palette_InterpolateBanks(((u8 *)p) + 0x3c8, gPaletteBankData + 0x60,
                                     gPaletteBankData + 0x3ec0, *((s32 *)(((u8 *)p) + 0x5a8)),
                                     *((s32 *)(((u8 *)p) + 0x5ac)));
            *((void **)(((u8 *)p) + 0x35c)) = ((u8 *)p) + 0x3c8;
            Palette_InterpolateBanks(((u8 *)p) + 0x3e8, gPaletteBankData + 0x40,
                                     gPaletteBankData + 0x3ea0, *((s32 *)(((u8 *)p) + 0x5a8)),
                                     *((s32 *)(((u8 *)p) + 0x5ac)));
            *((void **)(((u8 *)p) + 0x378)) = ((u8 *)p) + 0x3e8;
            i = 0;
            asm("" : "+r"(i));
            dest = ((u8 *)p) + 0x488;
            src = ((u8 *)p) + 0x408;
            do {
                Palette_InterpolateBanks(gUnk_03005040[i], src, dest, *((s32 *)(((u8 *)p) + 0x5a8)),
                                         *((s32 *)(((u8 *)p) + 0x5ac)));
                dest += 32;
                src += 32;
            } while ((++i) <= 3);
            if ((*((s32 *)(((u8 *)p) + 0x5a8))) > (1 << (*((s32 *)(((u8 *)p) + 0x5ac))))) {
                *((s32 *)(((u8 *)p) + 0x304)) = 1;
                *((s32 *)(((u8 *)p) + 0x34c)) = 1;
                *((s32 *)(((u8 *)p) + 0x398)) = 1;
            } else {
                *((s32 *)(((u8 *)p) + 0x304)) = 0;
                *((s32 *)(((u8 *)p) + 0x34c)) = 0;
                if ((*((s32 *)(((u8 *)p) + 0x5a8))) >
                    (1 << ((*((s32 *)(((u8 *)p) + 0x5ac))) - 1))) {
                    if ((*((s32 *)(((u8 *)p) + 0x5a8))) & 1) {
                        *((s32 *)(((u8 *)p) + 0x304)) = 1;
                        *((s32 *)(((u8 *)p) + 0x34c)) = 1;
                    }
                    if (((*((s32 *)(((u8 *)p) + 0x5a8))) >
                         (1 << ((*((s32 *)(((u8 *)p) + 0x5ac))) - 2))) &&
                        (Mod(*((s32 *)(((u8 *)p) + 0x5a8)), 3) == 0)) {
                        *((s32 *)(((u8 *)p) + 0x304)) = 1;
                        *((s32 *)(((u8 *)p) + 0x34c)) = 1;
                    }
                }
            }
            break;
        }

        case 1:
            if ((++(*((s32 *)(((u8 *)p) + 0x5a0)))) > 49)
                *((s32 *)(((u8 *)p) + 0x398)) = 2;
            break;

        case 2: {
            struct PackedVec pos;
            struct PackedVec vel;
            register s32 i asm("r8");
            i = 4;
            do {
                pos.x = ((*((s32 *)(((u8 *)p) + 0x268))) >> 12) + 50;
                pos.y = 138;
                pos.z = 50;
                vel.x = (Random() & 3) - 1;
                t1 = -(Random() & 1);
                vel.y = t1;
                vel.z = 0;
                sub_08113398(&pos, &vel, *((void **)(((u8 *)p) + 0x388)),
                             *((void **)(((u8 *)p) + 0x38c)), 5, 0, 0, 0, 2, 42, 0, 512, 1, 0);
                (*((s32 *)(((u8 *)p) + 0x59c)))++;
            } while ((--i) >= 0);
            if ((*((s32 *)(((u8 *)p) + 0x59c))) > 23)
                *((s32 *)(((u8 *)p) + 0x398)) = 3;
            break;
        }

        case 3:
            *((u8 *)(((u8 *)p) + 0xaf)) = 1;
            break;
    }

    ActorDisplay_UpdateScreenAnchor(input);
    ActorDisplay_UpdateForegroundSprite(input);
    ActorDisplay_UpdateAccentSprite(input);
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActors_UpdatePaletteDispersal);
#endif
