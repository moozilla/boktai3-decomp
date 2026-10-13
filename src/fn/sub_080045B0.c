#include "global.h"
#include "scene_object.h"

void sub_0822B2F8(u32);

/* Play the sound for pushing an object of this type. */
void sub_080045B0(s32 type)
{
    switch (type) {
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
            sub_0822B2F8(0x142);
            break;
        case 1:
            sub_0822B2F8(0x137);
            break;
        default:
            sub_0822B2F8(0x142);
            break;
    }
}
