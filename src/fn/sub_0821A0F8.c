#include "global.h"

struct Task { u8 pad[4]; struct Task *next; u8 pad8[0xE]; u8 f16; };
struct List { struct Task *head; u32 pad; };
extern struct List gUnk_03005280[];
s32 sub_0821A0C0(struct Task *);

void sub_0821A0F8(s32 prio)
{
    struct List *l = gUnk_03005280;
    s32 i = 0;
    do {
        struct Task *t = l->head;
        s32 ni = i + 1;
        struct List *nl = l + 1;
        for (; t != 0; t = t->next) {
            if (t->f16 < prio)
                sub_0821A0C0(t);
        }
        i = ni;
        l = nl;
    } while (i <= 13);
}
