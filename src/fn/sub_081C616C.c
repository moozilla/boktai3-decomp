#include "global.h"

struct S {
    u8 pad[0x364];
    u32 f364;
    u8 pad2[0x370 - 0x368];
    s32 f370;
};
void Script_Call(u32, u32);
void Task_Kill(struct S *);

void sub_081C616C(struct S *p)
{
    p->f370++;
    if (p->f370 > 0x20) {
        if (p->f364) {
            Script_Call(p->f364, 0);
            Task_Kill(p);
        }
    }
}
