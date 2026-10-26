#include "global.h"
#include "ram.h"
struct SpritePairEntry {
    u8 pad0[0x44];
    u8 animData[0x1c];
    u32 spriteFlags;
    u8 pad64[3];
    u8 spriteMode;
    u8 pad68[0x14];
    u32 bounds[2];
    u8 pad84[8];
    u8 anim[0x10];
    void *sequence;
    u8 padA0[0x2e];
    u8 active;
    u8 padCF[0xd];
};
struct SpritePairPool {
    u8 pad0[0x18];
    struct SpritePairEntry entries[2];
    u32 counter;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Memory_Clear(void *, u32);
s32 SpritePairPool_Initialize(struct SpritePairPool *pool)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        struct SpritePairEntry *entry = &pool->entries[i];
        if (!AnimData_Load(entry->animData, 0x409c))
            return -1;
        Sprite_Init(&entry->spriteFlags, entry->animData, 0);
        entry->sequence = ScriptProc_Start(0x922e, 0x1c22);
        Anim_SetSequence(entry->anim, entry->sequence, 9, 0, 0);
        AnimData_SelectPaletteBank(entry->animData, 0xe4);
        Memory_Clear(entry->bounds, 8);
        entry->spriteFlags |= 0x11;
        entry->spriteMode = 1;
        entry->active = 0;
    }
    pool->counter = 0;
    gSpritePairPool = pool;
    return 0;
}
