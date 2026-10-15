#include "scene_panel_rig.h"
#define ScenePanelRig_TickKnockback sub_0805A2BC
static inline s32 scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
void ScenePanelRig_TickKnockback(struct ScenePanelRig *input)
{
    struct ScenePanelRig *rig = input;
    s32 dx, dz;
    u16 *zptr;
    rig->scale = 6;
    {
        u32 force = rig->contactForce;
        {
            s32 delta = scale(gUnk_086149C4[(rig->contactDirection + 64) & 255] * force);
            u16 *xptr = (void *)&rig->position.x;
            *xptr += delta;
        }
    }
    {
        u32 force = rig->contactForce;
        {
            s32 delta = scale(gUnk_086149C4[rig->contactDirection] * force);
            zptr = (void *)&rig->position.z;
            *zptr += delta;
        }
    }
    dx = rig->position.x - rig->origin.x;
    dz = (s16)*zptr - rig->origin.z;
    if (dx * dx + dz * dz > 490000 - 1) {
        {
            u16 *force = &rig->contactForce;
            if (*force < 30)
                *force = 30;
        }
        {
            u8 *direction = &rig->contactDirection;
            *direction += 128;
        }
    } else {
        u16 *force = &rig->contactForce;
        *force = *force * 3 >> 2;
    }
    if (rig->contactForce == 0)
        ScenePanelRig_SetState(rig, 8);
}
