#define gParticleRandomIndex  gEffectRandomTableIndex
#define gParticleRandomValues gUnk_086141BC
// Tentative family and semantic names; layouts follow observed field offsets.
#include "global.h"
#include "ram.h"
struct ParticleOffset {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 unused : 16;
};
struct ParticleEmitter {
    u8 unknown[0xdc];
    s16 position[3];
    u8 padE2[0xee - 0xe2];
    u8 variant;
};
extern const u16 gUnk_086141BC[];
void sub_08113398(s16 *, struct ParticleOffset *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                  u32, u32);
static inline u32 ParticleRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
void ActorParticles_SpawnAlternatingBurst(struct ParticleEmitter *p)
{
    s32 count, mode, i;
    struct ParticleOffset offset;
    switch (p->variant) {
        case 0:
            count = 1;
            mode = 0;
            break;
        case 2:
            count = 1;
            mode = 1;
            break;
        case 1:
            count = 4;
            mode = 0;
            break;
        default:
            count = 1;
            mode = 0;
            break;
    }
    for (i = 0; i < 4; i++) {
        s32 size, flip;
        offset.x = (ParticleRandom() & 7) - 3;
        offset.y = -(ParticleRandom() & 7) - 3;
        offset.z = (ParticleRandom() & 7) - 3;
        if (!(i & 1)) {
            size = 512;
            flip = 1;
        } else {
            size = 1024;
            flip = 0;
        }
        sub_08113398(p->position, &offset, 0xae8e, 0x87e6, count, 0, mode, flip, 260, 40, 1, size,
                     1, 4);
    }
}
