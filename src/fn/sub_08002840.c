#include "global.h"
#include "background_controller.h"

void *sub_08002840(u16 id, struct ScrollingBackgroundConfig *c)
{
    struct ScrollingBackground *p;
    if (c->priority == 0)
        p = sub_08219FBC(9, 0xf8);
    else
        p = sub_08219FBC(0xb, 0xf8);
    if (p != 0) {
        sub_0821A04C(p, ScrollingBackground_Update, sub_08002658);
        if (ScrollingBackground_Initialize(p, id, c) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
