#include "scene/collision/packed_contact_vector.h"
// Tentative family and semantic names; layouts follow observed field offsets.
#include "global.h"
#include "engine/ram.h"
#include "objects/items/collectible.h"
extern const u16 gUnk_086141BC[];
void SceneDebris_Create(s16 *, struct PackedContactVector *, u32, u32, u32, u32, u32, u32, u32, u32,
                        u32, u32, u32, u32);
static inline u32 ParticleRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
void ActorParticles_SpawnAlternatingBurst(struct Collectible *item)
{
    s32 count, mode, i;
    struct PackedContactVector offset;
    switch (item->type) {
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
        SceneDebris_Create(&item->x, &offset, 0xae8e, 0x87e6, count, 0, mode, flip, 260, 40, 1,
                           size, 1, 4);
    }
}
