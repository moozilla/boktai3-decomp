#include "group_objects.h"
int GroupObjects_Initialize(struct GroupObjects *m, int id)
{
    struct GroupPiece *p;
    int i;
    m->active = 0;
    m->actorId = 0;
    m->actor = 0;
    m->animation = ScriptProc_Start(0x922E, 0x5BB7);
    {
        void *first = gUnk_030042E4 + 0x5A60;
        void *second = gUnk_030042E4 + 0x5AA0;
        sub_08221110(m->palette, first, second, 64, 6);
    }
    p = m->pieces;
    for (i = 0; i < 16; i++, p++)
        BoardPiece_Initialize(m, p, i);
    SceneMessages_RegisterReceiver(&m->queue, id, 10);
    return 0;
}
