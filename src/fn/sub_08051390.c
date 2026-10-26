#include "directional_force.h"

extern u16 gUnk_0200052C;
extern u16 gUnk_02000550;
extern u16 gUnk_0200053C;
void DirectionalForce_Destroy(void)
{
    gUnk_0200052C = 0;
    gUnk_02000550 = 0;
    gUnk_0200053C = 0;
    gDirectionalForce = 0;
}
