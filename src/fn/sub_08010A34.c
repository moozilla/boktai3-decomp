#include "group_objects.h"

void sub_08010A34(struct GroupObjects *m, struct GroupPiece *p)
{
    if (p->active != 0) {
        p->active = 0;
        p->sprite.flags |= 1;
        ActorShadow_Disable(p->transform);
    }
}
