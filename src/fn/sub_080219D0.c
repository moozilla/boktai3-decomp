// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u8 *gGameState;
void Camera_RequestShake(int);
void SceneActors_SpawnAmbientParticle(void *);
typedef struct {
    int unknown;
    u32 flags;
} Damage;
void SceneActors_UpdateEffects(u8 *p)
{
    Damage *damage;
    if (*(s16 *)(p + 0x94) > 0)
        *(int *)(gGameState + 0x618) = *(s16 *)(p + 0x94);
    else
        *(int *)(gGameState + 0x618) = 0;
    if (p[0xb4]) {
        Camera_RequestShake(p[0xb5]);
        p[0xb4]--;
    }
    if (p[0x154] && (*(int *)(p + 0xe4) & 3) == 0)
        SceneActors_SpawnAmbientParticle(p);
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
