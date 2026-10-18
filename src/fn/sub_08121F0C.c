#include "global.h"

void SceneEffect863A_LoadResources(void);
extern s32 gUnk_02000138;

s32 SceneSpriteBurst_InitManager(s32 value)
{
    SceneEffect863A_LoadResources();
    gUnk_02000138 = value;
    return 0;
}
