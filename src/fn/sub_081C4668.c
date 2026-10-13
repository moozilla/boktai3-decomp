#include "global.h"

struct S {
    u8 pad[0x5c];
    u32 f5c;
    u32 f60;
    u8 pad2[4];
    s32 f68;
};
void sub_081C439C(struct S *);
void Script_Call(u32, u32);
void Task_Kill(u32);
void sub_081C44E0(struct S *);

void sub_081C4668(struct S *p)
{
    sub_081C439C(p);
    p->f68++;
    if (p->f68 > 0x3f) {
        if (p->f5c) {
            Script_Call(p->f5c, 0);
            Task_Kill(p->f60);
            Task_Kill((u32)p);
        } else {
            sub_081C44E0(p);
        }
    }
}
