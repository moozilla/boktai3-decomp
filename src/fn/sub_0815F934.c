#include "global.h"
#include "scene_vec.h"
#include "engine_globals.h"

#define gParticleRandomIndex gRandomIndex
#define gParticleRandomTable gUnk_0203B400
struct PlayerOrbitParticle {
    u32 flags;
    u8 unknown4[0x14];
    struct SceneVec position;
    u8 unknown20[8];
    u8 active, angle, riseSpeed, expandDelay, animationPhase, angularSpeed, lifetime, state;
    u16 radius, unknown32;
    s16 velocityX, velocityY, velocityZ, unknown3A;
};
struct PlayerOrbitEffect {
    u8 unknown0[0xF8];
    void *resource;
    struct PlayerOrbitParticle particles[16];
    u16 frame;
};
extern u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
static inline s32 TruncateOrbitOffset(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
void PlayerOrbitEffect_SpawnParticle(struct PlayerOrbitEffect *effect, struct SceneVec *position,
                                     s32 angle)
{
    s32 i;
    struct PlayerOrbitParticle *p;
    for (i = 0; i < 16; i++) {
        if (!effect->particles[i].active) {
            break;
        }
    }
    if (i < 16) {
        {
            const u16 *randomTable;
            u32 *randomIndex;
            u32 range;
            u32 mask;
            u32 index;
            p = &effect->particles[i];
            p->flags &= ~1;
            p->position = *position;
            randomTable = gUnk_0203B400;
            randomIndex = &gRandomIndex;
            index = *randomIndex;
            index++;
            mask = 0x3FF;
            index &= mask;
            {
                u32 random = randomTable[*(&index)];
                s32 x = (u16)p->position.x;
                x -= 63;
                range = 0x7F;
                random &= range;
                p->position.x = x + random;
            }
            index = (index + 1) & mask;
            *randomIndex = index;
            {
                u32 random = randomTable[index];
                s32 z = (u16)p->position.z;
                z -= 63;
                random &= range;
                p->position.z = z + random;
            }
        }
        p->position.y += 128;
        SpriteDrawable_SelectTileFrame(p, effect->resource, 4);
        p->velocityX = TruncateOrbitOffset(6 * gUnk_086149C4[(angle + 64) & 255]);
        p->velocityZ = TruncateOrbitOffset(6 * gUnk_086149C4[angle & 255]);
        p->animationPhase = 0;
        p->state = 1;
        {
            u32 index = (gRandomIndex + 1) & 0x3FF;
            gRandomIndex = index;
            {
                u32 random = *(u16 *)((index << 1) + (u32)gUnk_0203B400);
                u32 range = 7;
                p->lifetime = (random & range) + 8;
            }
        }
        p->active = 1;
    }
}
