#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
#define EffectSpritePool_Initialize        sub_081A4760
#define EffectSpritePool_InitializeEntries sub_081A4718
struct EffectSpriteEntry {
    u32 flags;
    u8 unknown4[0x48];
    u32 owner;
    u16 state;
    u8 unknown52[6];
    u8 frame, mode;
    u8 unknown5a[2];
};
struct EffectSpritePool {
    u8 unknown0[0x18];
    u8 animation[2][0x1c];
    void *procedure;
    u32 count;
    u8 active;
    u8 unknown59[3];
    struct EffectSpriteEntry entries[32];
};
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void *ScriptProc_Start(u16, u16);
s32 sub_081A4718(struct EffectSpritePool *);
s32 sub_081A4760(struct EffectSpritePool *pool)
{
    pool->active = 0;
    pool->count = 0;
    AnimData_Load(pool->animation[0], 0x306e);
    AnimData_SelectPaletteBank(pool->animation[0], 0x2f1);
    AnimData_Load(pool->animation[1], 0x3071);
    AnimData_SelectPaletteBank(pool->animation[1], 0x2f1);
    pool->procedure = ScriptProc_Start(0x922e, 0xd56f);
    if (EffectSpritePool_InitializeEntries(pool) < 0)
        return -1;
    return 0;
}
