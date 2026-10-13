#include "global.h"
#include "background_controller.h"

void *sub_08001CB0(u16 id, struct BackgroundConfig *c)
{
    struct Background *p;
    if (c->priority == 0)
        p = sub_08219FBC(9, 0x140);
    else
        p = sub_08219FBC(0xb, 0x140);
    if (p != 0) {
        sub_0821A04C(p, Background_Update, sub_08001AD0);
        if (Background_Initialize(p, id, c) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
