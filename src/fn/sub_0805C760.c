#include "scene_panel_commands.h"
#define ScenePanel_IsPresent sub_0805C760
u32 sub_0805C760(void)
{
    u32 present = (u32)gUnk_020004A4;
    if (present != 0)
        present = 1;
    return present;
}
