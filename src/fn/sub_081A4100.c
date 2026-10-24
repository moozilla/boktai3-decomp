#include "global.h"

struct MotionVector {
    s16 x, y, z, pad;
};
struct MotionEntry {
    u32 flags;
    u8 unknown4[0xC];
    u16 tile;
    u8 unknown12[0xA];
    struct MotionVector position;
    u8 unknown24[8];
    u8 animation[0x10];
    struct MotionVector savedPosition, delta;
    struct MotionVector *origin;
    u16 lifetime, frame, radius, height;
    u8 type, mode;
    u16 angularStep;
};
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
extern u32 gUnk_03005308;
extern struct MotionPool *gUnk_02000240;
struct MotionEntry *Hresvelgr_FindInactiveEffect(void);
void Sprite_ApplyAnimation(void *, void *);
s32 Mod(s32, s32);
void Anim_SetSequence(void *, void *, s32, s32, s32);
void MotionEntry_UpdatePosition(void *);
static inline s32 randomValue(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
struct MotionEntry *MotionEntry_Create(struct MotionVector *origin, u32 type, u32 mode,
                                       u32 lifetime)
{
    struct MotionCounts counts = gUnk_0824F770;
    struct MotionPool *pool = gUnk_02000240;
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
        MotionEntry_UpdatePosition(entry);
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
