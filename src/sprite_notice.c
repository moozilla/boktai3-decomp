#include "sprite_notice.h"
#include "global.h"
#include "scene_particle_burst.h"
#include "scene_particle_step.h"
#include "ram.h"

/* SceneScheduledEffect_Shutdown: Empty hook following the effect manager reset; shutdown role is tentative */

void SceneScheduledEffect_Shutdown(void)
{
}

/* Steps an animated sprite's display sequence (SceneParticle_StepSprite, as in SceneParticleBurst_UpdateSlot); sets result=2 when the sequence ends. */

void SequencedSprite_StepAnimation(struct SpriteNoticeManager *owner, struct SpriteNotice *slot)
{
    struct SpriteNotice *notice = slot;
    struct SceneParticleEffect *sprite = &notice->sprite;
    if (notice->entered) {
        notice->sprite.flags &= ~1;
        notice->entered = 0;
    }
    if (notice->animation.index == 1 && notice->animation.elapsed == 0)
        sprite->position.y += 32;
    {
        const u16 *graphics = owner->spriteData;
        if (SceneParticle_StepSprite(&notice->animation, sprite, graphics))
            notice->state = 2;
    }
}

void Effect_Remove(void *);

void SceneAnimatedEffect_ReleaseEntry(struct SpriteNoticeManager *manager,
                                      struct SpriteNotice *notice)
{
    u8 *indexByte;
    s32 index;
    Effect_Remove(notice);
    notice->state = 0;
    indexByte = (u8 *)&notice->index;
    index = *(s8 *)indexByte;
    manager->occupied &= ~(1 << index);
    Memory_Clear(notice, sizeof(*notice));
    *indexByte = 0xff;
}

static inline u8 SpriteNotice_IsOccupied(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}

struct SpriteNotice *SpriteNotice_AllocateSlot(struct SpriteNoticeManager *pool)
{
    s32 i;
    for (i = 0; i < 32; i++) {
        if (!SpriteNotice_IsOccupied(&pool->occupied, i)) {
            struct SpriteNotice *slot = &pool->slots[i];
            slot->index = i;
            pool->occupied |= 1 << i;
            return slot;
        }
    }
    return 0;
}

void ParticleSprite_Initialize(void *, void *, u32);

void Effect_SetOrigin(void *, s32, s32);

s32 SpriteNotice_Create(const struct ContactPosition *position, u32 mode)
{
    struct SpriteNoticeManager *manager;
    struct SpriteNotice *slot;
    if (!position)
        return -1;
    manager = (void *)gSpriteNoticeManager;
    if (!manager) {
        manager = SceneTrailEffect_EnsurePool();
        if (!manager)
            return -1;
    }
    slot = SpriteNotice_AllocateSlot(manager);
    if (!slot)
        return -1;
    slot->sprite.position = *position;
    ParticleSprite_Initialize(slot, manager->spriteData, 0);
    Effect_SetOrigin(slot, -8, -8);
    {
        u32 zero = 0;
        Anim_SetSequence(&slot->animation, manager->animationData, zero, 0, zero);
    }
    slot->sprite.flags |= 1;
    slot->sprite.priority = mode;
    slot->state = 1;
    slot->entered = 1;
    return 0;
}

extern void *gUnk_08610728[];

s32 sub_08249240(void *, void *, void *);

static inline u32 *SpriteNotice_GetOccupiedMask(struct SpriteNoticeManager *manager)
{
    return &manager->occupied;
}

s32 SpriteNotice_TickManager(struct SpriteNoticeManager *manager)
{
    struct SpriteNotice *start;
    s32 i;
    u32 *mask;
    void **table;
    u8 *state;
    struct SpriteNotice *slot;
    if (!manager->occupied)
        return 0;
    i = 0;
    start = manager->slots;
    mask = SpriteNotice_GetOccupiedMask(manager);
    table = gUnk_08610728;
    state = &manager->slots[0].state;
    slot = start;
    for (; i < 32;) {
        if (SpriteNotice_IsOccupied(mask, i))
            sub_08249240(manager, slot, table[*state]);
        state += sizeof(*slot);
        slot++;
        i++;
    }
    return 0;
}

static inline u8 SpriteNotice_TestSlot(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

u32 SceneAnimatedEffect_OnTaskDestroy(struct SpriteNoticeManager *manager)
{
    s32 index = 0;
    u32 *occupied = &manager->occupied;
    struct SpriteNotice *notice = manager->slots;
    for (; index < 32; notice++, index++) {
        if (SpriteNotice_TestSlot(occupied, index))
            Effect_Remove(notice);
    }
    {
        u32 z = 0;
        gSpriteNoticeManager = z;
    }
    return 0;
}

void SceneAnimatedEffect_LoadResources(struct SpriteNoticeManager *manager)
{
    manager->spriteData = AnimData_FindEntry(0x1C1A);
    manager->animationData = ScriptProc_Start(0x922E, 0xD1B8);
}

s32 SceneTrailEffect_InitPool(struct SpriteNoticeManager *manager)
{
    SceneAnimatedEffect_LoadResources(manager);
    gSpriteNoticeManager = (u32)manager;
    return 0;
}

struct SpriteNoticeManager *SceneTrailEffect_EnsurePool(void)
{
    struct SpriteNoticeManager *manager;
    if (gSpriteNoticeManager != 0)
        return (void *)gSpriteNoticeManager;
    manager = Task_Create(0xa, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))SpriteNotice_TickManager,
                          (void (*)(void *))SceneAnimatedEffect_OnTaskDestroy);
        if (SceneTrailEffect_InitPool(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

void SceneTrailEffect_ResetPool(void)
{
    gSpriteNoticeManager = 0;
}
