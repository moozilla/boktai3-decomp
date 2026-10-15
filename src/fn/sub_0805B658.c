#include "global.h"
extern u32 gUnk_02000580;
s32 sub_0805B318(u8 *);
void sub_0815C99C(u32);
void ScenePanel_SetEffectState(u8 *, u32);
void ScenePanel_CallPendingScript(u8 *, u32);
void sub_0805B658(u8 *p)
{
    if (sub_0805B318(p) != 0) {
        sub_0815C99C(gUnk_02000580);
        ScenePanel_SetEffectState(p, 3);
        ScenePanel_CallPendingScript(p, 0);
    }
}
