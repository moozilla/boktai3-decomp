#include "orbit_background.h"
void OrbitBackground_ResetOrbit(struct OrbitBackground *background)
{
    background->angle = gUnk_02000710->unk85E;
    background->radius = 300;
    background->blendFrame = 0;
}
