#include "global.h"

void sub_08013B74(u8 *);
void Collider_Remove(u8 *);
void Anim_Remove(u8 *);

void sub_08198B44(u8 *p)
{
    sub_08013B74(p + 0x88);
    Collider_Remove(p + 0xF4);
    if (*(p + 0x2C)) {
        Anim_Remove(p + 0x28);
    }
}
