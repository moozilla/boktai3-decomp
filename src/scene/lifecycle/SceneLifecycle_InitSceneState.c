#include "global.h"
#include "engine/video/render.h"
void Bg_ResetDisplay(void);
void Engine_ClearControlFlags(void);
void Terrain_ResetMap(void);
void Collider_EnsureManager(void);
void SceneLifecycle_InitSceneState(void)
{
    u32 m;
    gSpriteAttrFlags = 0;
    Bg_ResetDisplay();
    Engine_ClearControlFlags();
    Terrain_ResetMap();
    Collider_EnsureManager();
    m = 0x100;
    gDispCntShadow = m | gDispCntShadow;
}
