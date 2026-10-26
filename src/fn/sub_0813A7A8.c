#include "global.h"
#include "ram.h"
struct CompactSpriteEntry {
    u8 animData[0x1c];
    u32 spriteFlags;
    u8 pad20[3];
    u8 spriteMode;
    u8 pad24[0x14];
    u32 bounds[2];
    u8 pad40[8];
    u8 anim[0x18];
    u32 value;
};
struct CompactSpritePool {
    u8 pad0[0x18];
    struct CompactSpriteEntry entries[2];
    void *sequence;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Memory_Clear(void *, u32);
s32 CompactSpritePool_Initialize(struct CompactSpritePool *pool)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        struct CompactSpriteEntry *entry = &pool->entries[i];
        if (!AnimData_Load(entry->animData, 0x409c))
            return -1;
        Sprite_Init(&entry->spriteFlags, entry->animData, 0);
        AnimData_SelectPaletteBank(entry->animData, 0xe4);
        pool->sequence = ScriptProc_Start(0x922e, 0x1c22);
        Anim_SetSequence(entry->anim, pool->sequence, 6, 0, 0);
        Memory_Clear(entry->bounds, 8);
        pool->entries[i].value = 0;
        pool->entries[i].spriteFlags |= 0x11;
        pool->entries[i].spriteMode = 1;
    }
    gCompactSpritePool = pool;
    return 0;
}
