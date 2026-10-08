#include "global.h"

struct Task { u8 pad[4]; struct Task *next; u8 pad8[8]; u16 f10; };
struct List { struct Task *head; u32 pad; };
extern struct List gUnk_03005280[];

struct Task *sub_0821A130(u16 id)
{
    struct List *l = gUnk_03005280;
    s32 i = 0;
    do {
        struct Task *t = l->head;
        for (; t != 0; t = t->next) {
            if (t->f10 == id)
                return t;
        }
        i++;
        l++;
    } while (i <= 13);
    return 0;
}
