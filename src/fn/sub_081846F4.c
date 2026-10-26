#include "global.h"

/* tentative: 4-state charge attack with fade, collider arm and camera shake */

struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
struct Vec8 {
    u32 a, b;
};
struct Collider {
    u8 pad[0x14];
    struct Vec8 v14;
    struct Vec8 v1c;
    u8 pad2[0x14];
    u32 flags38;
};
struct PlayerPos {
    u8 pad[0x30];
    s16 x;
    u16 pad2;
    s16 z;
};

extern u32 gUnk_0824EE80[];
extern u32 gUnk_0824EE90[];
extern const struct Vec8 gUnk_0824EEA0;
extern const struct Vec8 gUnk_0824EEA8;
extern u32 gUnk_0824EEB0[];
extern struct PlayerPos *gGameState;
void Armor_SetAnimationFromStart(u32 a, u32 b);
void Sound_PlaySongChecked(u32 a);
void Armor_StartFastWhiteFade(void *);
void Armor_StartPaletteTransition(void *);
void ActorPalette_Reset(void *);
void Collider_SetAttackPowerAndForce(void *, s32, s32);
void Camera_RequestShake(s32);
s32 Math_Atan2(s32, s32);

void ActorController_UpdateChargeAttackTransition(u8 *p)
{
    u8 *s = p + 0xB40;
    u8 st = *s;

    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EE80[((p[0xB39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xB41] = st;
            ((struct ActorAnimationLayer *)(p + 0x238))->flags |= four;
            ((struct ActorAnimationLayer *)(p + 0x1E4))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x28C))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
            {
                struct ActorRenderFlags *render = (void *)(p + 0x180);
                u32 one = 1;
                render->flags |= one;
            }
            *(u16 *)(p + 0xB32) &= ~0x20;
            Sound_PlaySongChecked(0xC1);
            break;
        }
        case 1:
            if (p[0xB43] != 0) {
                struct Collider *c;
                Armor_SetAnimationFromStart((u32)p,
                                            (u32)&gUnk_0824EE90[((p[0xB39] + 0x60) >> 6) & 3]);
                {
                    u32 next = *s + 1;
                    u32 zero = 0;
                    *s = next;
                    p[0xB41] = zero;
                    p[0xB42] = zero;
                }
                Armor_StartFastWhiteFade(p);
                p[0xB44] = 20;
                *(u16 *)(p + 0x196) &= ~4;
                c = (struct Collider *)(p + 0x190);
                {
                    u32 mask = 0x40000;
                    c->flags38 |= mask;
                }
                Collider_SetAttackPowerAndForce(c, 0, 0x50);
                c->v1c = gUnk_0824EEA8;
                c->v14 = gUnk_0824EEA0;
                p[0x1D2] = Math_Atan2(gGameState->x - *(s16 *)(p + 0x48),
                                      gGameState->z - *(s16 *)(p + 0x4C));
                Sound_PlaySongChecked(0x1BA);
            }
            break;
        case 2:
            if (p[0xB42] > 7) {
                Armor_SetAnimationFromStart((u32)p,
                                            (u32)&gUnk_0824EEB0[((p[0xB39] + 0x60) >> 6) & 3]);
                {
                    u32 next = *s + 1;
                    u32 zero = 0;
                    *s = next;
                    p[0xB41] = zero;
                }
                Armor_StartPaletteTransition(p);
                {
                    u16 *flags = (u16 *)(p + 0x196);
                    u32 old = *flags;
                    u32 set = 4;
                    set |= old;
                    *flags = set;
                }
            }
            break;
        case 3:
            if (p[0xB43] != 0) {
                *s = 0xFF;
                p[0xB41] = 1;
                ActorPalette_Reset(p);
            }
            break;
    }
    if (p[0xB44] != 0) {
        Camera_RequestShake(0x80);
        p[0xB44]--;
    }
}
