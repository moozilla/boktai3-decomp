// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"

struct Pair {
    u32 a, b;
};
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void Task_Kill(void *);
void SceneNpc_SpawnTransitionParticle(void *, s32);
void SceneNpc_UpdateTransitionParticles(void *);

#define RAND() (gUnk_0203B400[gRandomIndex = (gRandomIndex + 1) & 0x3FF])

s32 SceneNpc_UpdateTransitionEffect(u8 *p)
{
    if (*(u32 *)(p + 0x74) == 0)
        *(u32 *)(p + 0x70) += 1;
    *(struct Pair *)(p + 0x34) = *(struct Pair *)(p + 0x60);
    switch (*(u32 *)(p + 0x6c)) {
        case 0:
            *(u32 *)(p + 0x74) = 0;
            if (*(s32 *)(p + 0x70) > 0x37) {
                *(u32 *)(p + 0x6c) = 1;
                *(u32 *)(p + 0x70) = 0;
                *(u32 *)(p + 0x74) = 1;
            }
            break;
        case 1: {
            u32 t = *(u32 *)(p + 0x74);
            const u16 *tab;
            u32 *cur;
            u32 zero = 0;
            *(u32 *)(p + 0x74) = zero;
            if (t != 0)
                p[0x1f] = 2;
            tab = gUnk_0203B400;
            cur = &gRandomIndex;
            *cur = (*cur + 1) & 0x3FF;
            {
                s32 m = Mod(tab[*cur], 0x28);
                u32 v = *(u16 *)(p + 0x34);
                v -= 0x14;
                *(u16 *)(p + 0x34) = v + m;
            }
            *cur = (*cur + 1) & 0x3FF;
            {
                s32 m = Mod(tab[*cur], 0x28);
                u32 v = *(u16 *)(p + 0x36);
                v -= 0x14;
                *(u16 *)(p + 0x36) = v + m;
            }
            *cur = (*cur + 1) & 0x3FF;
            {
                s32 m = Mod(tab[*cur], 0x28);
                u32 v = *(u16 *)(p + 0x38);
                v -= 0x14;
                *(u16 *)(p + 0x38) = v + m;
            }
            if (*(s32 *)(p + 0x70) > 0x3a) {
                *(u32 *)(p + 0x6c) = 2;
                *(u32 *)(p + 0x70) = zero;
                *(u32 *)(p + 0x74) = 1;
            }
            break;
        }
        case 2:
            *(u32 *)(p + 0x74) = 0;
            *(u16 *)(p + 0x62) -= 0x14;
            if (*(s8 *)(p + 0x20) > 0x30) {
                p[0x20] -= 2;
                p[0x21] -= 2;
            }
            if (*(s32 *)(p + 0x70) > 6) {
                *(u32 *)(p + 0x6c) = 3;
                *(u32 *)(p + 0x70) = 0;
                *(u32 *)(p + 0x74) = 1;
            }
            break;
        case 3:
            *(u32 *)(p + 0x74) = 0;
            if (*(s8 *)(p + 0x20) <= 0x3f) {
                p[0x20] += 4;
                p[0x21] += 4;
            }
            *(u16 *)(p + 0x62) += 0x64;
            if (*(s32 *)(p + 0x70) > 2 && Mod(*(s32 *)(p + 0x70), 3) == 0)
                SceneNpc_SpawnTransitionParticle(p, -1);
            if (*(s32 *)(p + 0x70) > 0x1d) {
                *(u32 *)(p + 0x6c) = 4;
                *(u32 *)(p + 0x70) = 0;
                *(u32 *)(p + 0x74) = 1;
            }
            break;
        case 4: {
            u32 t = *(u32 *)(p + 0x74);
            *(u32 *)(p + 0x74) = 0;
            if (t != 0)
                *(u32 *)(p + 0x18) |= 1;
            if (*(s32 *)(p + 0x70) > 300)
                Task_Kill(p);
            return 0;
        }
    }
    {
        s32 m = Mod(*(s32 *)(p + 0x68), 3);
        u32 f;
        if (m <= 1) {
            if (m >= 0) {
                f = 1;
                asm("" : "+r"(f));
            } else
                f = 0;
        } else
            f = 0;
        *(u16 *)(p + 0x28) = f;
    }
    SceneNpc_UpdateTransitionParticles(p);
    *(s32 *)(p + 0x68) += 1;
    return 0;
}
