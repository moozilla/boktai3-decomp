#include "objects/pile_driver/pile_driver_rig.h"
void PileDriverRig_InitMovement(struct PileDriverRig *rig)
{
    s32 dx = rig->position.x - rig->origin.x, dz = rig->position.z - rig->origin.z;
    u32 angle = Math_Atan2(dx, dz);
    rig->angle = angle << 4;
    rig->turnStep = 0;
    if (angle - 32 <= 63 || angle - 160 <= 63)
        rig->distance = Div(dz << 12, gUnk_086149C4[angle & 255]);
    else
        rig->distance = Div(dx << 12, gUnk_086149C4[(angle + 64) & 255]);
}
