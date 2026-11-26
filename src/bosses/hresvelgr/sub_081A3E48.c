#include "global.h"

struct Vec {
    s16 x, y, z, pad;
};
struct EffectEntry {
    u32 flags;
    u8 unknown4[0x18];
    struct Vec position;
    u8 unknown24[8];
    u8 animation[0x10];
    u8 collider[0xA8];
    struct Vec velocity;
    u8 shadow[0x6C];
    u8 frame, speed, trailCount;
    u8 unknown15B[3];
    u16 lifetime;
    u8 unknown160[0x178 - 0x160];
};
struct EffectPool {
    u8 unknown0[0x18];
    u8 animationData[0x1C];
    void *sequence;
    u8 unknown38[8];
    u16 attackA, attackB, attackC;
    u8 unknown46[2];
    struct EffectEntry entries[3];
};
void Sprite_Init(void *, void *, s32);
void Anim_SetSequence(void *, void *, s32, s32, s32);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, s32, s32);
void ScaledShadow_Disable(void *);
s32 Hresvelgr_InitHomingShotCollider(void *, s32, s32, s32);
s32 EffectPool_CreateEntries(struct EffectPool *pool)
{
    struct EffectEntry *entry = pool->entries;
    s32 i, result;
    for (i = 0; i < 3; i++) {
        void *shadow;
        Sprite_Init(entry, pool->animationData, 0);
        Anim_SetSequence(entry->animation, pool->sequence, 3, 0, 0);
        entry->flags |= 1;
        shadow = entry->shadow;
        ScaledShadow_Initialize(shadow, &entry->position, 64, 64, 64, 64, 512, 1, 3, 1, 0);
        ScaledShadow_SetGraphic(shadow, 0x1C1B, 7);
        ScaledShadow_Disable(shadow);
        result =
            Hresvelgr_InitHomingShotCollider(entry, pool->attackA, pool->attackB, pool->attackC);
        entry->lifetime = 0;
        entry->speed = 0;
        entry++;
    }
    return result;
}
