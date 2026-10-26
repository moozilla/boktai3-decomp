#include "global.h"
#include "render.h"
void sub_082157EC(void);
void Engine_ClearControlFlags(void);
void Terrain_ResetMap(void);
void sub_08220ADC(void);
void SceneLifecycle_InitSceneState(void)
{
    u32 m;
    gSpriteAttrFlags = 0;
    sub_082157EC();
    Engine_ClearControlFlags();
    Terrain_ResetMap();
    sub_08220ADC();
    m = 0x100;
    gDispCntShadow = m | gDispCntShadow;
}
