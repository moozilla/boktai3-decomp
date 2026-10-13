#include "group_objects.h"
int GroupObjects_Destroy(struct GroupObjects *m)
{
    struct GroupPiece *p = m->pieces;
    int i;
    for (i = 0; i < 16; i++, p++)
        sub_0800FA40(m, p, i);
    sub_08020D08(&m->queue);
    return 0;
}
