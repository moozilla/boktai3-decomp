#include "ballistic_sprite_effect.h"

void SoundDelay_Update(struct ParticleEmitter *p)
{
    if (p->entered) {
        p->timer = 0;
        Sound_PlaySong(0x1b5);
        p->entered = 0;
    }
    if (p->timer > 30) {
        p->update = ParticleEmitter_UpdateFlight;
        p->entered = 1;
    } else {
        p->timer++;
    }
}

extern const u16 gUnk_086141BC[];

void NativeParticle_CreateWithFlag(void *, s32, void *, s32, s32, s32, s32);

static inline s32 FrameDelay(s32 speed, s32 weight)
{
    return (speed * weight) >> 6;
}

static inline u32 BallisticSpriteEffect_StepAnimation(struct ContactSwarmSprite *sprite,
                                                      struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(sprite, state);
}

static inline u32 RandomValue(void)
{
    return gUnk_086141BC[gEffectRandomTableIndex];
}

// Tentative: update the initial drift and particle trail of a ballistic sprite.

void BallisticSpriteEffect_UpdateLaunch(struct ParticleEmitter *p)
{
    if (p->entered) {
        p->sprite.flags &= ~1;
        p->phase = 0;
        p->timer = 0;
        p->entered = 0;
        Sound_PlaySong(0x1b4);
    }
    p->position.x += p->velocity.x + gUnk_086149C4[p->phase] * (p->timer >> 3);
    p->position.y += p->velocity.y - gUnk_086149C4[p->phase] * (p->timer >> 1);
    p->position.z += p->velocity.z;
    p->phase++;
    p->timer++;
    {
        struct ScenePoolVec32 *pos = &p->position;
        struct ContactPosition *dst = &p->sprite.position;
        u32 yOff = p->heightOffset;
        dst->x = (pos->x >> 12) - gSceneCamera->x;
        dst->y = ((s32)(pos->z + (pos->y + yOff)) >> 12) + 0x60;
        dst->z = pos->z >> 12;
    }
    if (p->timer <= 19) {
        struct {
            u32 x : 16, y : 16, z : 16, pad : 16;
        } offset;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.x = (RandomValue() & 15) - 7;
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        offset.y = (RandomValue() & 15) - 7;
        NativeParticle_CreateWithFlag(&p->sprite.position, 0, &offset, 1, 1, 0, 0);
    }
    BallisticSpriteEffect_StepAnimation(&p->sprite, &p->animation);
    {
        u16 *timer = &p->timer;
        if (*timer > 150) {
            p->update = SoundDelay_Update;
            p->entered = 1;
        }
    }
}

s32 BallisticSpriteEffect_UpdateManager(struct BallisticSpriteEffectManager *manager)
{
    s32 slot;
    for (slot = 0; slot < 4; slot++) {
        if (manager->entries[slot].active && manager->entries[slot].update)
            Callback_InvokeActor(&manager->entries[slot], manager->entries[slot].update);
    }
    return 0;
}

s32 BallisticSpriteEffect_DestroyManager(struct BallisticSpriteEffectManager *manager)
{
    s32 remaining;
    u32 zero;
    struct ContactSwarmSprite *sprite = &manager->entries[0].sprite;
    remaining = 3;
    do {
        if (sprite->unknown4[0] != 0)
            Sprite_Remove(sprite);
        sprite = (void *)((u8 *)sprite + sizeof(manager->entries[0]));
    } while (--remaining >= 0);
    zero = 0;
    gSceneProcSprites = zero;
    return 0;
}

s32 SceneProcSprites_Initialize(struct BallisticSpriteEffectManager *p)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        void *proc;
        if (!AnimData_Load(p->entries[i].resource, 0x409c))
            return -1;
        Sprite_Init(&p->entries[i].sprite, p->entries[i].resource, 0);
        proc = ScriptProc_Start(0x922e, 0x1c22);
        p->sequenceResource = proc;
        Anim_SetSequence(&p->entries[i].animation, proc, 10, 0, 0);
        AnimData_SelectPaletteBank(p->entries[i].resource, 0xe4);
        Memory_Clear(&p->entries[i].sprite.position, 8);
        p->entries[i].active = 0;
        p->entries[i].sprite.flags |= 0x11;
        p->entries[i].sprite.priority = 1;
    }
    gSceneProcSprites = p;
    return 0;
}

struct BallisticSpriteEffectManager *BallisticSpriteEffect_EnsureManager(void)
{
    struct BallisticSpriteEffectManager *manager;
    if (gSceneProcSprites != 0)
        return gSceneProcSprites;
    manager = Task_Create(0xa, 0x39c);
    if (manager != 0) {
        Task_SetCallbacks(manager, BallisticSpriteEffect_UpdateManager,
                          BallisticSpriteEffect_DestroyManager);
        if (SceneProcSprites_Initialize(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

void BallisticSpriteEffect_OnAnimationComplete(void)
{
}

void Position_GetHeightAdjustedCoordinates(void *, u32, void *);

void SpatialIndex_InitEntry(void *, void *, void *, void *, u32, u32, u32, u32, u32, void (*)(void),
                            void *);

void SceneProcSprites_SubmitBounds(struct ParticleEmitter *entry, void (*callback)(void), u32 kind,
                                   u32 mode)
{
    u32 position[2];
    union BallisticSpriteBounds bounds, offset;
    Position_GetHeightAdjustedCoordinates(&entry->position, 0, position);
    bounds.axis.horizontal.low = 14;
    bounds.axis.horizontal.high = 16;
    bounds.axis.vertical.low = 14;
    offset.words[0] = -bounds.axis.horizontal.high << 16;
    offset.words[1] &= 0xFFFF0000;
    SpatialIndex_InitEntry(entry->spatialEntry, position, &bounds, &offset, 12, mode, 5, kind, 0x28,
                           callback, entry);
}

s32 SceneProcSprites_CreateEntry(struct ScenePoolVec32 *position, s32 heightOffset,
                                 s32 horizontalSpeed, u32 owner, u16 *parameters)
{
    struct BallisticSpriteEffectManager *manager;
    u8 *existing = gSceneProcSprites;
    s32 slot;
    if (existing)
        manager = (void *)existing;
    else {
        manager = (void *)BallisticSpriteEffect_EnsureManager();
        if (!manager)
            return -1;
    }
    for (slot = 0; slot < 4; slot++) {
        if (!manager->entries[slot].active) {
            manager->entries[slot].sprite.flags |= 1;
            manager->entries[slot].position = *position;
            manager->entries[slot].origin = *position;
            manager->entries[slot].active = 1;
            Memory_Clear(&manager->entries[slot].velocity, 16);
            manager->entries[slot].velocity.x = horizontalSpeed + 0x2000;
            manager->entries[slot].velocity.y = -0x1000;
            manager->entries[slot].velocity.z = 0;
            manager->entries[slot].heightOffset = heightOffset;
            manager->entries[slot].initialVelocity = manager->entries[slot].velocity.x;
            manager->entries[slot].owner = owner;
            SceneProcSprites_SubmitBounds(&manager->entries[slot],
                                          BallisticSpriteEffect_OnAnimationComplete, parameters[3],
                                          owner + 1);
            ((struct BallisticSpriteEffectManager *)((u8 *)manager +
                                                     slot * sizeof(manager->entries[0])))
                ->entries[0]
                .update = BallisticSpriteEffect_UpdateLaunch;
            manager->entries[slot].entered = 1;
            return slot;
        }
    }
    return -1;
}
