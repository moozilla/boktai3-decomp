#include "global.h"
#include "engine.h"

void *sub_0821A130(u16);

s32 sub_0821A168(u16 id)
{
    void *t = sub_0821A130(id);
    s32 r;
    if (t)
        r = Task_Kill(t);
    else
        r = -1;
    return r;
}
