#include "scene_departure_display.h"

void SpriteParticle_InitParams(void *);
void SpriteParticle_SetMotionParams(void *, u32, u32, u32, u32);
void SpriteParticle_SetSpriteResource(void *, u32, u32, s32);
void SpriteParticle_SetAnimationTrack(void *, u32, u32);
void SpriteParticle_SetPlanarVelocity(void *, s32, s32);
s32 SpriteParticle_Emit(void *, void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

static inline u32 NextRandom(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[gRandomIndex];
}

union DepartureJitterSource {
    const u16 *table;
    u16 halves[2];
};
static inline u32 ReadDepartureJitter(const union DepartureJitterSource *source)
{
    return NextRandom(source->table);
}
void SingletonSlot7_UpdateShakingDeparture(struct DepartureActor *input, s32 frame)
{
    struct DepartureActor *actor = input;
    if (input->entered) {
        input->entered = 0;
        input->foreground.mode = 1;
        input->accent.mode = 1;
        Sound_PlaySong(0x596);
    }
    input->shakeX = 0;
    input->shakeZ = 0;
    if (frame < 60) {
        union DepartureJitterSource jitter;
        jitter.halves[0] = (u16)RANDOM_TABLE_ADDR;
        jitter.halves[1] = RANDOM_TABLE_ADDR >> 16;
        input->shakeX = (ReadDepartureJitter(&jitter) & 7) - 4;
        input->shakeZ = (ReadDepartureJitter(&jitter) & 7) - 4;
    } else if (input->foreground.mode && input->accent.mode) {
        input->dispersalTimer += 2;
    }
    if ((frame & 1) && frame > 60) {
        struct ContactPackedVector position;
        u32 params[0x34 / sizeof(u32)];
        s32 dx, dz;
        u16 originX;
        u32 roll;
        roll = NextRandom(gRandomTable);
        originX = actor->foreground.sprite.position.x;
        position.x = (roll & 31) + originX - 16;
        position.y = actor->foreground.sprite.position.y;
        position.z = 100;
        dx = ((NextRandom(gRandomTable) & 15) - 8) << 10;
        dz = -((NextRandom(gRandomTable) & 7) << 10);
        SpriteParticle_InitParams(params);
        SpriteParticle_SetMotionParams(params, 0, 60, 0, 0);
        if ((s32)(NextRandom(gRandomTable) & 255) > 128) {
            SpriteParticle_SetSpriteResource(params, actor->effectResource, 0, 0x1f5);
            SpriteParticle_SetAnimationTrack(params, actor->effectGraphics, 5);
        } else {
            SpriteParticle_SetSpriteResource(params, actor->effectResource, 0, 0x1f5);
            SpriteParticle_SetAnimationTrack(params, actor->effectGraphics, 6);
        }
        SpriteParticle_SetPlanarVelocity(params, dx, dz);
        SpriteParticle_Emit(params, &position);
    }
    if (actor->dispersalTimer == 58) {
        actor->burstTimer = 90;
        actor->burstX = actor->foreground.sprite.position.x;
        actor->burstY = 155;
        Sound_PlaySong(0x146);
        Sound_PlaySong(0x597);
    }
    if (actor->dispersalTimer >= 60)
        actor->dispersalTimer = 60;
    if (frame > 90 && Sprite_BlinkUntilExpired(&actor->foreground, frame - 90, 45, 90))
        input->finished = 1;
}
