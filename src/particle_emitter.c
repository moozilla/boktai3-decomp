#include "particle_emitter.h"

void TimedSpriteEffect_RequestStop(u32 slot)
{
    struct ParticleEmitterDisplay *display = gSpriteSlots;
    if (display != 0 && slot <= 3) {
        if (display->slots[slot].active != 0)
            display->slots[slot].stopRequested = 1;
    }
}

void ParticleEmitter_ResetDisplay(void)
{
    gSpriteSlots = 0;
}

s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);

u8 ParticleEmitter_IsBelowHeightLimit(struct ParticleEmitter *p)
{
    struct ScenePoolVec32 *position = &p->position;
    s32 height = Terrain_GetHeightAtFixedPosition(position);
    s32 offset = p->heightOffset;
    s32 y = position->y;
    s32 limit = position->z + (y + offset);
    if (height < (limit >> 12) + 0x70)
        return 1;
    else
        return 0;
}

void ParticleEmitter_OnStop(struct ParticleEmitter *emitter)
{
    u8 *entered = &emitter->entered;
    if (*entered != 0) {
        u8 *active = &emitter->active;
        u32 zero = 0;
        *active = zero;
        *entered = zero;
        emitter->update = 0;
    }
}

s32 Mod(s32, s32);

void Collider_SetFixedPositionWithYOffset(void *, void *, s32);

void SpatialIndex_InsertEntry(void *);

// confidence=low: emits a trail while moving left; emitter identity is tentative.

extern const u16 gUnk_086141BC[];

void SpriteEffect_CreateAtPosition(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, u32);

#define RAND()                                                                                     \
    (gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023,                               \
     gUnk_086141BC[gEffectRandomTableIndex])

void ParticleEmitter_UpdateScatterTrail(struct ParticleEmitter *actor)
{
    struct ScenePoolVec32 pos;
    if (actor->entered) {
        actor->sprite.flags |= 1;
        actor->timer = 0;
        actor->entered = 0;
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
    Collider_SetFixedPositionWithYOffset(actor->spatialEntry, &actor->position, 0);
    SpatialIndex_InsertEntry(actor->spatialEntry);
    if (actor->timer > 60) {
        actor->update = ParticleEmitter_OnStop;
        actor->entered = 1;
    }
}

s32 Div(s32, s32);

static inline void GetPosition(struct ScenePoolVec32 *out, s32 index)
{
    if (index > 1) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
    } else {
        struct ScenePoolPlayer *p = gParticipantActors[index];
        if (!p) {
            out->x = 0;
            out->y = 0;
            out->z = 0;
        } else {
            out->x = p->position[0];
            out->y = p->position[1];
            out->z = p->position[2];
        }
    }
}

static inline void GetOffset(struct ScenePoolVec32 *out, s32 index)
{
    if (index > 1) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
    } else {
        struct ScenePoolPlayer *p = gParticipantActors[index];
        if (!p) {
            out->x = 0;
            out->y = 0;
            out->z = 0;
        } else {
            out->x = p->viewportOriginX;
            out->y = p->viewportOriginY;
            out->z = p->viewportOriginZ;
        }
    }
}

static inline u32 TestMode(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

void ParticleEmitter_UpdateFlight(struct ParticleEmitter *p)
{
    struct ScenePoolVec32 pos, offset;
    if (p->entered) {
        if (TestMode(0x800)) {
            GetPosition(&pos, 1 - p->owner);
            GetOffset(&offset, 1 - p->owner);
        } else {
            GetPosition(&pos, 0);
            GetOffset(&offset, 0);
        }
        p->position.x = pos.x - offset.x + 0xf0000;
        p->position.y = pos.y - pos.z - 0x3ffc0;
        p->position.z = pos.z;
        p->sprite.flags &= ~1;
        p->velocity.x = Div(pos.x - p->position.x, 30) + p->initialVelocity;
        p->velocity.y = Div(pos.y - p->position.y, 20) + 0x2000;
        p->heightOffset = 0;
        p->phase = 0;
        p->timer = 0;
        p->entered = 0;
    }
    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;
    p->timer++;
    {
        struct ScenePoolVec32 *pos = &p->position;
        struct ContactPosition *dst = &p->sprite.position;
        u32 yOff = p->heightOffset;
        dst->x = (pos->x >> 12) - gSceneCamera->x;
        dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
        dst->z = pos->z >> 12;
    }
    PlayerCue_StepSequenceEnded(&p->sprite, &p->animation);
    p->sprite.flags |= 12;
    if (ParticleEmitter_IsBelowHeightLimit(p)) {
        p->update = ParticleEmitter_UpdateScatterTrail;
        p->entered = 1;
    }
}
