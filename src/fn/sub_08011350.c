#include "message_menu.h"
struct Position {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void MessageMenu_InitializeTransition(struct MessageMenu *m)
{
    struct Position zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    sub_0821983C(m->transition, &m->resource, 57, 16, 0, 0, 60, &zero);
}
