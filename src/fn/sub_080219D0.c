// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneActors_UpdateEffects sub_080219D0
extern u8 *gUnk_02000710;
void sub_082260A4(int);
void sub_08021D8C(void *);
typedef struct {
    int unknown;
    u32 flags;
} Damage;
void SceneActors_UpdateEffects(u8 *p)
{
    Damage *damage;
    if (*(s16 *)(p + 0x94) > 0)
        *(int *)(gUnk_02000710 + 0x618) = *(s16 *)(p + 0x94);
    else
        *(int *)(gUnk_02000710 + 0x618) = 0;
    if (p[0xb4]) {
        sub_082260A4(p[0xb5]);
        p[0xb4]--;
    }
    if (p[0x154] && (*(int *)(p + 0xe4) & 3) == 0)
        sub_08021D8C(p);
    if (p[0x155] && (*(u32 *)(p + 0xe4) & 3) <= 1) {
        u32 m = 0x10;
        asm("" : : "r"(m));
        *(u32 *)(p + 0x9c) |= 0x10;
    }
    if (p[0x156]) {
        u32 m = 0x10;
        asm("" : : "r"(m));
        *(u32 *)(p + 0x9c) |= 0x10;
    }
    {
        u32 m = 0x40;
        asm("" : : "r"(m));
        if (*(u32 *)(p + 0x9c) & 0x40) {
            damage = (Damage *)(p + 0x15c);
            {
                u32 m = ~1u;
                asm("" : : "r"(m));
                damage->flags &= ~1;
            }
        } else {
            damage = (Damage *)(p + 0x15c);
            {
                u32 m = 1;
                asm("" : : "r"(m));
                damage->flags |= 1;
            }
        }
    }
}
