#include "scene_panel_interaction.h"
#define ScenePanel_IsWithinHorizontalRadius sub_0805B28C
s32 ScenePanel_IsWithinHorizontalRadius(const struct ContactPosition *first,
                                        const struct ContactPosition *second, s32 radius)
{
    s32 x = first->x - second->x;
    s32 z = first->z - second->z;
    if (x > radius || z > radius || (s32)((u32)x * x + (u32)z * z) > (s32)((u32)radius * radius))
        return 0;
    else
        return 1;
}