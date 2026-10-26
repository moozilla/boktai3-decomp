#include "global.h"
#include "ram.h"

/* tentative: hides the sprite and spawns three randomly scattered bursts */

struct Vec3 {
    s32 x, y, z, w;
};
struct BurstActor {
    void (*update)(struct BurstActor *);
    u8 pad4[0x20 - 4];
    u32 spriteFlags;
    u8 pad24[0x9c - 0x24];
    struct Vec3 position;
    u8 padAC[0xd4 - 0xac];
    u8 active;
    u8 padD5[3];
    u16 timer;
};
s32 Mod(s32, s32);
void ParticleEmitter_OnStop(struct BurstActor *);
void Collider_SetFixedPositionWithYOffset(void *, void *, s32);
void SpatialIndex_InsertEntry(void *);
// confidence=low: emits a trail while moving left; emitter identity is tentative.
extern const u16 gUnk_086141BC[];
void Sound_PlaySong(u32);
void SpriteEffect_CreateAtPosition(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, u32);

#define RAND()                                                                                     \
    (gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023,                               \
     gUnk_086141BC[gEffectRandomTableIndex])

void ParticleEmitter_UpdateScatterTrail(struct BurstActor *actor)
{
    struct Vec3 pos;
    if (actor->active) {
        actor->spriteFlags |= 1;
        actor->timer = 0;
        actor->active = 0;
    }
    if (Mod(actor->timer, 5) == 0) {
        pos = actor->position;
        pos.z += ((RAND() & 31) - 15) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        pos = actor->position;
        pos.z += ((RAND() & 31) - 15) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        pos = actor->position;
        pos.z += ((RAND() & 63) - 31) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        Sound_PlaySong(0x16e);
    }
    actor->position.x -= 0x4000;
    actor->timer++;
    Collider_SetFixedPositionWithYOffset((u8 *)actor + 0x5c, &actor->position, 0);
    SpatialIndex_InsertEntry((u8 *)actor + 0x5c);
    if (actor->timer > 60) {
        actor->update = ParticleEmitter_OnStop;
        actor->active = 1;
    }
}
