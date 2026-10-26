#include "global.h"
struct G {
    u8 pad[0x5a4];
    u32 f5a4;
};
extern struct G *gGameState;
extern u32 gUnk_030053E8;
u32 Script_GetValue(void);
s32 Script_SeekToKeyword(s32);
u8 *Script_GetPc(void);
void Script_SetEntryProcedure(s32);
s32 SceneLifecycle_LoadFromScript(void)
{
    u16 s = Script_GetValue();
    u32 *g = &gUnk_030053E8;
    *g = 1;
    if (Script_SeekToKeyword(0x6e) == 0 || (Script_GetPc() != 0 && Script_GetValue() == 0))
        *g |= 0x10;
    Script_SetEntryProcedure((s16)s);
    gGameState->f5a4 = s;
    return 0;
}
