#include "scene_panel_commands.h"
u32 ScenePanel_IsPresent(void)
{
    u32 present = (u32)gUnk_020004A4;
    if (present != 0)
        present = 1;
    return present;
}
