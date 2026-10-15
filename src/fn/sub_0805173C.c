#include "breakable_prop.h"
#define BreakableProp_Destroy sub_0805173C
void sub_0821D6D0(void *);
u32 BreakableProp_Destroy(struct BreakableProp *prop)
{
    Sprite_Remove(&prop->sprite);
    Collider_Remove(&prop->collider);
    sub_0821D6D0(prop->terrainRegistration);
}
