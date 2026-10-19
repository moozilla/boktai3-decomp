#include "global.h"
#define SceneMenu_PositionDisplay sub_0810C0BC
struct SceneMenuDisplayElement {
    u8 pad0[0x20]; s16 x, y; u8 pad24[0x3C];
};
struct SceneMenuDisplayPosition { s16 x, y; u8 pad4[0x5C]; };
struct SceneMenuDisplay {
    u8 pad0[0x20]; struct SceneMenuDisplayElement elements[7];
};
void sub_0810C0BC(struct SceneMenuDisplay *p)
{
    s32 i = 0;
    struct SceneMenuDisplayPosition *position = (void *)&p->elements[0].x;
    do {
        switch (i) {
        case 0:
            p->elements[0].x = i;
            p->elements[0].y = i;
            break;
        case 1:
            p->elements[1].x = 144;
            p->elements[1].y = 0;
            break;
        case 6:
            p->elements[6].x = 8;
            p->elements[6].y = 8;
            break;
        default:
            position->x = 0;
            position->y = 0;
            break;
        }
        position++;
        i++;
    } while (i <= 6);
}
