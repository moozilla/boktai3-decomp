#include "global.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "scene/collision/contact_position.h"
#include "bosses/hresvelgr/motion_entry_layout.h"

struct MotionPool {
    u8 unknown0[0x18];
    u8 animationA[0x1C], animationB[0x1C];
    void *sequence;
    u8 unknown54[4];
    u8 allocated;
};
struct MotionCounts {
    u32 count[3];
};
extern const struct MotionCounts gUnk_0824F770;
extern const u16 gUnk_08611F38[];
#define RANDOM_TABLE_ADDR 0x0203B400
struct MotionEntry *Hresvelgr_FindInactiveEffect(void);
void Sprite_ApplyAnimation(void *, void *);
s32 Mod(s32, s32);
void Anim_SetSequence(void *, void *, s32, s32, s32);
void HresvelgrMotion_UpdatePosition(void *);
static inline s32 randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
struct MotionEntry *HresvelgrMotion_Create(struct ContactPosition *origin, u32 type, u32 mode,
                                       u32 lifetime)
{
    struct MotionCounts counts = gUnk_0824F770;
    struct MotionPool *pool = gMotionEntryManager;
    struct MotionEntry *entry;
    if (type > 3 || mode > 1 || !(entry = Hresvelgr_FindInactiveEffect()))
        return NULL;
    pool->allocated++;
    entry->type = type;
    entry->mode = mode;
    if (entry->type == 3) {
        Sprite_ApplyAnimation(entry, pool->animationB);
        entry->tile = Mod(randomValue(), 4) + 4;
    } else {
        s32 choice = Mod((randomValue() >> 3) & 255, counts.count[entry->type]);
        Sprite_ApplyAnimation(entry, pool->animationA);
        Anim_SetSequence(entry->animation, pool->sequence, gUnk_08611F38[entry->type], (u8)choice,
                         0);
    }
    entry->lifetime = lifetime;
    if (!entry->mode) {
        entry->origin = origin;
        HresvelgrMotion_UpdatePosition(entry);
    } else {
        entry->origin = NULL;
        entry->savedPosition = *origin;
        entry->position = entry->savedPosition;
    }
    entry->flags &= ~1;
    if (pool->allocated & 1) {
        entry->flags |= 0x200;
        entry->flags &= ~0x400;
    } else {
        entry->flags |= 0x400;
        entry->flags &= ~0x200;
    }
    entry->frame = 0;
    return entry;
}
