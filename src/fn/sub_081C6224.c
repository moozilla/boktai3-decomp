#include "global.h"
#include "render.h"
#define gDisplayControlShadow gDispCntShadow
struct SceneBackgroundOwner {
    u8 unknown0[0x35c];
    void *script;
};
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, void *);
void SceneObject_StartBackgroundScript(struct SceneBackgroundOwner *p)
{
    u32 layers[1];
    void *script;
    u16 *control = &gDispCntShadow;
    u32 mask = 0xfffff0ff;
    u32 flags;
    mask &= *control;
    flags = mask | 0x400;
    *control = flags;
    script = ScriptProc_Start(0xc091, 0x3536);
    p->script = script;
    layers[0] = 12;
    Bg_SetupLayers(1, 0, script, 0, 0, 1, layers);
}
