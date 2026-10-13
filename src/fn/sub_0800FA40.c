#include "group_objects.h"

u32 sub_0800FA40(struct GroupObjects *m, struct GroupPiece *p, int index)
{
    sub_08013684(p->transform);
    Sprite_Remove(&p->sprite);
}
