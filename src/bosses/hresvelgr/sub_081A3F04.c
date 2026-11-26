#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
struct EffectPool {
    u8 unknown0[0x18];
    u8 animation[0x1c];
    void *procedure;
    u8 unknown38[4];
    u32 count;
    u16 parameters[3];
};
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void *ScriptProc_Start(u16, u16);
s32 EffectPool_CreateEntries(struct EffectPool *);
s32 EffectPool_Initialize(struct EffectPool *controller, s32 a, s32 b, s32 c)
{
    controller->count = 0;
    AnimData_Load(controller->animation, 0x306e);
    AnimData_SelectPaletteBank(controller->animation, 0x2f0);
    controller->procedure = ScriptProc_Start(0x922e, 0xd56f);
    controller->parameters[0] = a;
    controller->parameters[1] = b;
    controller->parameters[2] = c;
    if (EffectPool_CreateEntries(controller) < 0)
        return -1;
    return 0;
}
