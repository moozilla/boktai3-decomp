#include "global.h"
#include "scene_vec.h"
#include "ram.h"

struct TileTrigger {
    u8 unk_00[0x84];
    struct SceneVec position;
    u8 unk_8C[0x2C];
    u16 sequence;
};
extern u16 gUnk_0203B400[];
void SpriteParticle_CreateDefaultVariant4384(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);

static inline u32 NextRandom(void)
{
    const u16 *table = gUnk_0203B400;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

void TileTrigger_EmitParticles(struct TileTrigger *p)
{
    struct SceneVec position;
    u32 one;
    if (gEngineFlags & 0x800)
        return;
    switch (p->sequence) {
        case 0:
        case 1:
            one = 1;
            position = p->position;
            position.x = position.x + 92 - (NextRandom() & 63);
            position.z = position.z + 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 4, 0, 0, 32, 64, one, one,
                                                    512);
            position = p->position;
            position.x = position.x - 92 - (NextRandom() & 63);
            position.z = position.z - 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 4, 0, 0, 32, 64, one, one,
                                                    512);
            position = p->position;
            position.x = position.x - 92 - (NextRandom() & 63);
            position.z = position.z + 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 4, 0, 0, 32, 64, one, one,
                                                    512);
            position = p->position;
            position.x = position.x + 92 - (NextRandom() & 63);
            position.z = position.z - 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 2, 0, 0, 32, 64, one, one,
                                                    512);
            position = p->position;
            position.x = position.x - 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 2, 0, 0, 32, 64, one, one,
                                                    512);
            position = p->position;
            position.x = position.x + 92 - (NextRandom() & 63);
            SpriteParticle_CreateDefaultVariant4384(&position, 8, 8, 4, 4, 0, 0, 32, 64, one, one,
                                                    512);
            break;
    }
}
