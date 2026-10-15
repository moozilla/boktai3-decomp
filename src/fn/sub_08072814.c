#include "scene_actor_script.h"
u16 SceneActorScript_GetManagerField62(void)
{
    u16 value;
    if (gUnk_020004A8)
        value = gUnk_020004A8->field62;
    else
        value = 0;
    return value;
}
