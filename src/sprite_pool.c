#include "sprite_pool.h"

int SpritePool_Update(struct SpritePool *pool)
{
    struct SpritePoolSlot *slot = pool->slots;
    int index = 15;
    do {
        if (slot->active) {
            if (slot->callback) {
                if (slot->callback(pool, slot)) {
                    Sprite_Remove(&slot->sprite);
                    slot->active = 0;
                    slot->id = 0;
                }
                slot->age++;
            } else {
                Sprite_Remove(&slot->sprite);
                slot->active = 0;
                slot->id = 0;
            }
        }
        index--;
        slot++;
    } while (index >= 0);
    return 0;
}

u32 SpritePool_Destroy(struct SpritePool *pool)
{
    struct SpritePoolSlot *slot = pool->slots;
    s32 index = 0xf;
    do {
        if (slot->sprite.unknown4[0] != 0)
            Sprite_Remove(&slot->sprite);
        index--;
        slot++;
    } while (index >= 0);
    gSpritePoolManager = 0;
    return 0;
}

s32 SpritePool_Initialize(struct SpritePool *pool)
{
    struct SpritePoolSlot *slot;
    struct TransientSpriteGraphics *graphics;
    s32 index;
    gSpritePoolManager = (u32)pool;
    slot = pool->slots;
    index = 0xf;
    do {
        graphics = &slot->graphics;
        AnimData_Load(graphics, 0xda6d);
        Sprite_InitUnlinked(&slot->sprite, graphics, 0);
        index--;
        slot++;
    } while (index >= 0);
    return 0;
}

struct SpritePool *SpritePool_Create(void)
{
    struct SpritePool *pool = (struct SpritePool *)gSpritePoolManager;
    if (pool == 0) {
        pool = Task_Create(10, sizeof(struct SpritePool));
        if (pool != 0) {
            Task_SetCallbacks(pool, (void (*)(void *))SpritePool_Update,
                              (void (*)(void *))SpritePool_Destroy);
            if (SpritePool_Initialize(pool) < 0) {
                Task_Kill(pool);
                return 0;
            }
        }
    }
    return pool;
}

struct SpritePoolSlot *SpritePool_FindFreeSlot(struct SpritePool *pool)
{
    struct SpritePoolSlot *slot = pool->slots;
    s32 index = 0;
    struct SpritePoolSlot *result;
    do {
        if (slot->active != 0) {
            index++;
            slot++;
            continue;
        }
        result = slot;
        goto end;
    } while (index <= 0xf);
    result = 0;
end:
    return result;
}

struct SpritePoolSlot *SpritePool_Find(struct SpritePool *pool, u32 id)
{
    struct SpritePoolSlot *slot;
    int index;
    if (!id)
        return 0;
    slot = pool->slots;
    for (index = 0; index < 16; index++, slot++)
        if (slot->active && slot->id == id)
            return slot;
    return 0;
}

struct SpritePoolSlot *SpritePool_Allocate(void)
{
    struct SpritePool *pool = (struct SpritePool *)gSpritePoolManager;
    struct SpritePoolSlot *slot;
    if (pool != 0 || (pool = SpritePool_Create()) != 0) {
        slot = SpritePool_FindFreeSlot(pool);
        if (slot != 0) {
            Sprite_ResetRenderDefaults(&slot->sprite);
            return slot;
        }
    }
    return 0;
}

void SpritePool_SetResource(struct SpritePoolSlot *slot, u32 id)
{
    if (gSpritePoolManager)
        AnimData_Load(&slot->graphics, id);
}

void SpritePool_SetFrame(struct SpritePoolSlot *slot, u32 frame)
{
    if (gSpritePoolManager != 0)
        slot->sprite.frame = frame;
}

void SpritePool_SetPalette(struct SpritePoolSlot *slot, u32 id)
{
    if (gSpritePoolManager)
        AnimData_SelectPaletteBank(&slot->graphics, id);
}

void SpritePool_SetPosition(struct SpritePoolSlot *slot, const struct ContactPosition *position)
{
    if (gSpritePoolManager != 0)
        slot->sprite.position = *position;
}

void SpritePool_StartAnimation(struct SpritePoolSlot *slot, u32 resource, u32 sequence, u32 row,
                               u32 flags)
{
    if (gSpritePoolManager != 0) {
        void *data = ScriptProc_Start(0x922e, (u16)resource);
        slot->sequenceResource = data;
        Anim_SetSequence(&slot->sequence, data, (u16)sequence, (u8)row, (u8)flags);
    }
}

void SpritePool_SetAnimationFrame(struct SpritePoolSlot *slot, u32 sequence, u32 row, u32 flags)
{
    if (gSpritePoolManager)
        Anim_SetSequence(&slot->sequence, slot->sequenceResource, (u16)sequence, (u8)row,
                         (u8)flags);
}

int SpritePool_AdvanceAnimation(struct SpritePoolSlot *slot)
{
    if (!gSpritePoolManager)
        return 0;
    return PlayerCue_StepSequenceEnded(&slot->sprite, &slot->sequence);
}

u32 SpritePool_Activate(struct SpritePoolSlot *slot, SpritePoolCallback callback, void *context)
{
    struct SpritePool *manager = (struct SpritePool *)gSpritePoolManager;
    u32 id;
    if (!manager || slot->active)
        return 0;
    if (slot->sprite.flags & 0x80)
        Sprite_LinkToList(&slot->sprite, 1);
    else
        Sprite_LinkToList(&slot->sprite, 0);
    slot->callback = callback;
    slot->context = context;
    slot->age = 0;
    slot->active = 1;
    id = ++manager->generation;
    id = (id << 28) | (u32)slot;
    slot->id = id;
    return id;
}

void SpritePool_Remove(u32 id)
{
    if (gSpritePoolManager) {
        struct SpritePoolSlot *slot = SpritePool_Find((struct SpritePool *)gSpritePoolManager, id);
        if (slot) {
            Sprite_Remove(&slot->sprite);
            slot->active = 0;
            slot->id = 0;
        }
    }
}
