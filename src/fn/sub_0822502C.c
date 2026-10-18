#include "global.h"
void sub_082157EC(void);
void Engine_ClearControlFlags(void);
void Terrain_ResetMap(void);
void sub_08220ADC(void);
extern u32 gUnk_0300522C;
extern u16 gUnk_03004BD8;
void SceneLifecycle_InitSceneState(void)
{
    u32 m;
    gUnk_0300522C = 0;
    sub_082157EC();
    Engine_ClearControlFlags();
    Terrain_ResetMap();
    sub_08220ADC();
    m = 0x100;
    gUnk_03004BD8 = m | gUnk_03004BD8;
}
