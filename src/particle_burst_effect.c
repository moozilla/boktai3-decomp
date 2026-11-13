#include "particle_burst_effect.h"

void ParticleBurstEffect_SetStateCallback(struct BurstEntry *effect,
                                          void (*update)(struct BurstEntry *))
{
    effect->update = update;
    effect->entered = 1;
}

extern const u16 gUnk_086141BC[];

void SpriteEffect_CreateAtPosition(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, u32);

#define RAND()                                                                                     \
    (gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023,                               \
     gUnk_086141BC[gEffectRandomTableIndex])

void SpriteEffect_HideWithScatterBurst(struct BurstEntry *actor)
{
    struct BurstPos pos;
    if (actor->entered) {
        actor->sprite.flags |= 1;
        actor->active = 0;
        actor->update = 0;
        actor->entered = 0;
        pos = actor->position;
        pos.x += ((RAND() & 15) - 7) << 12;
        pos.z += ((RAND() & 15) - 7) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        pos = actor->position;
        pos.x += ((RAND() & 15) - 7) << 12;
        pos.z += ((RAND() & 15) - 7) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        pos = actor->position;
        pos.x += ((RAND() & 15) - 7) << 12;
        pos.z += ((RAND() & 15) - 7) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        Sound_PlaySong(0x195);
    }
}

extern const u8 gUnk_086108E8[];

void Collider_SetFixedPositionWithYOffset(void *, struct BurstPos *, s32);

void SpatialIndex_InsertEntry(void *);

void ParticleBurstEffect_TickActive(struct BurstEntry *p)
{
    s32 level;
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->entered = 0;
        Memory_Clear(p->velocity, sizeof(p->velocity));
        Collider_SetFixedPositionWithYOffset(p->collider, &p->position, p->yOffset);
    }
    if (p->timer) {
        p->timer--;
    } else {
        if (p->duration < 30)
            level = 3;
        else if (p->duration < 50)
            level = 2;
        else if (p->duration < 90)
            level = 1;
        else
            goto skip;
        Anim_SetSequence(&p->anim, p->data, 7, (u8)p->value, 0);
        p->value ^= 1;
        p->timer = gUnk_086108E8[level];
    }
skip: {
    struct BurstPos *pos = &p->position;
    struct ContactPosition *dst = &p->sprite.position;
    u32 yOff = (u32)p->yOffset;
    dst->x = (pos->x >> 12) - gSceneCamera->x;
    dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
    dst->z = pos->z >> 12;
}
    PlayerCue_StepSequenceEnded(&p->sprite, &p->anim);
    SpatialIndex_InsertEntry(p->collider);
    if (--p->duration == 0)
        ParticleBurstEffect_SetStateCallback(p, SpriteEffect_HideWithScatterBurst);
}

void ParticleBurstEffect_UpdateFalling(struct BurstEntry *p)
{
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->entered = 0;
        Memory_Clear(p->velocity, sizeof(p->velocity));
    }
    p->velocity[1] += 0x200;
    p->yOffset += p->velocity[1];
    if (p->yOffset >= 0) {
        if (!p->state) {
            Sound_PlaySong(0x3e6);
            p->state = 1;
        }
        p->yOffset = 0;
        ParticleBurstEffect_SetStateCallback(p, ParticleBurstEffect_TickActive);
    }
    {
        struct BurstPos *pos = &p->position;
        struct ContactPosition *dst = &p->sprite.position;
        s32 yOff = p->yOffset;
        dst->x = (pos->x >> 12) - gSceneCamera->x;
        dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
        dst->z = pos->z >> 12;
    }
    PlayerCue_StepSequenceEnded(&p->sprite, &p->anim);
    Collider_SetFixedPositionWithYOffset(p->collider, &p->position, p->yOffset);
    SpatialIndex_InsertEntry(p->collider);
}

s32 CallbackPool_Update(struct ParticleBurstManager *p)
{
    s32 i;
    for (i = 0; i < 6; i++) {
        if (p->entries[i].active) {
            void *callback = p->entries[i].update;
            if (callback)
                sub_08249240(&p->entries[i], i * sizeof(p->entries[0]), callback);
        }
    }
    p->frame++;
    return 0;
}

s32 ParticleBurstEffect_DestroyManager(void)
{
    s32 i;
    for (i = 0; i <= 5; i++)
        CallbackPool_RemoveEntry(i);
    return gSceneProcParticles = 0;
}

s32 SceneProcParticles_Initialize(struct ParticleBurstManager *p)
{
    s32 i;
    for (i = 0; i < 6; i++) {
        struct BurstEntry *s = &p->entries[i];
        void *proc;
        if (!AnimData_Load(s->resource, 0x409c))
            return -1;
        Sprite_Init(&s->sprite, s->resource, 0);
        proc = ScriptProc_Start(0x922e, 0x1c22);
        s->data = proc;
        Anim_SetSequence(&s->anim, proc, 7, 0, 0);
        AnimData_SelectPaletteBank(s->resource, 0xe4);
        Memory_Clear(&s->sprite.position, 8);
        s->sprite.flags |= 0x11;
        s->sprite.priority = 2;
        s->active = 0;
    }
    p->frame = 0;
    gSceneProcParticles = p;
    return 0;
}

struct ParticleBurstManager *ParticleBurstEffect_EnsureManager(void)
{
    struct ParticleBurstManager *p;
    if (gSceneProcParticles != 0)
        return gSceneProcParticles;
    p = Task_Create(0xa, 0x52c);
    if (p != 0) {
        Task_SetCallbacks(p, CallbackPool_Update, ParticleBurstEffect_DestroyManager);
        if (SceneProcParticles_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

s32 CallbackPool_MarkNextOfKind(struct ParticleBurstManager *p, s32 first, s32 kind, u8 *visited)
{
    s32 i;
    for (i = first; i < 6; i++) {
        if (p->entries[i].active && !visited[i] && p->entries[i].id == kind) {
            visited[i] = 1;
            return 1;
        }
    }
    return 0;
}

u8 SceneProcParticles_TestCapacity(struct ParticleBurstManager *p, u32 kind, s32 requested)
{
    u32 key;
    s32 groups = 0, freeCount = 0;
    u8 visited[6];
    s32 i;
    key = kind & 0xff;
    Memory_Clear(visited, 6);
    for (i = 0; i < 6; i++) {
        if (!p->entries[i].active)
            freeCount++;
        else if ((u8)p->entries[i].id == key) {
            if (!visited[i]) {
                visited[i] = 1;
                CallbackPool_MarkNextOfKind(p, i + 1, p->entries[i].id, visited);
                groups++;
            }
        }
    }
    if (groups <= 2 && freeCount >= requested)
        return 0;
    return 1;
}

void ParticleBurstEffect_OnAnimationComplete(struct BurstEntry *p)
{
    ParticleBurstEffect_SetStateCallback(p, SpriteEffect_HideWithScatterBurst);
}

void Position_GetHeightAdjustedCoordinates(void *, u32, void *);

void SpatialIndex_InitEntry(void *, void *, void *, void *, u32, u32, u32, u32, u32,
                            void (*)(struct BurstEntry *), void *);

void SceneProcParticles_SubmitBounds(struct BurstEntry *entry,
                                     void (*callback)(struct BurstEntry *), u32 kind, u32 mode)
{
    u32 position[2];
    union ParticleBurstBounds bounds, offset;
    Position_GetHeightAdjustedCoordinates(&entry->position, entry->yOffset, position);
    bounds.axis.horizontal.low = 12;
    bounds.axis.horizontal.high = 8;
    bounds.axis.vertical.low = 8;
    offset.words[0] = 0;
    offset.words[1] &= 0xFFFF0000;
    SpatialIndex_InitEntry(entry->collider, position, &bounds, &offset, 12, mode, 5, kind, 12,
                           callback, entry);
}

static inline u32 SpritePool_MakeKey(u32 counter, u32 id)
{
    u32 mask = 0xff;
    id &= mask;
    counter &= mask;
    counter <<= 8;
    id |= counter;
    return id;
}

s32 SpritePool_StartEntry(struct BurstPos *position, s32 heightOffset, u32 unused,
                          struct ParticleBurstParameters *params, u32 id)
{
    struct ParticleBurstManager *pool;
    s32 i;
    u32 key;
    if (!gSceneProcParticles) {
        pool = ParticleBurstEffect_EnsureManager();
        if (!pool)
            return -1;
    } else
        pool = gSceneProcParticles;
    key = pool->frame;
    key = SpritePool_MakeKey(key, id);
    if (SceneProcParticles_TestCapacity(pool, key, 1))
        return -1;
    for (i = 0; i < 6; i++) {
        if (!pool->entries[i].active) {
            pool->entries[i].position = *position;
            pool->entries[i].yOffset = heightOffset;
            pool->entries[i].duration = params->duration;
            pool->entries[i].active = 1;
            pool->entries[i].state = 0;
            pool->entries[i].value = 0;
            key = pool->frame;
            key = SpritePool_MakeKey(key, id);
            pool->entries[i].id = key;
            Anim_SetSequence(&pool->entries[i].anim, pool->entries[i].data, 7, 0, 0);
            SceneProcParticles_SubmitBounds(
                &pool->entries[i], ParticleBurstEffect_OnAnimationComplete, params->value, id + 1);
            ParticleBurstEffect_SetStateCallback(&pool->entries[i],
                                                 ParticleBurstEffect_UpdateFalling);
            Sound_PlaySong(0x3e5);
            return i;
        }
    }
    return -1;
}

void CallbackPool_RemoveEntry(u32 i)
{
    struct ParticleBurstManager *p = gSceneProcParticles;
    if (p && i < 6 && p->entries[i].sprite.unknown4[0]) {
        p->entries[i].active = 0;
        p->entries[i].id = 0;
        p->entries[i].update = 0;
        Sprite_Remove(&p->entries[i].sprite);
    }
}
