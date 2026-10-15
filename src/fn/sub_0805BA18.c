#include "global.h"
extern u8 *gUnk_02000488;
void ScenePanelRig_SetState(u8 *, u32);
void ScenePanel_SetEffectState(u8 *, u32);
void sub_0805BA18(u8 *p)
{
    if (gUnk_02000488[0x255] == 0xd) {
        ScenePanelRig_SetState(p + 0xc6c, 1);
        ScenePanel_SetEffectState(p, 0xa);
    }
}
