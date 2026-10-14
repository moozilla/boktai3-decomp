#include "global.h"
struct Actor {
    u8 pad0[0xe];
    u16 flags;
    u8 pad10[0x4c];
    u32 id;
    u8 pad60[8];
    u8 pos[8];
    u8 pad70[0x34];
    u16 linkId;
    u8 pada6[2];
    void *linkPos;
    u8 padac[0x28];
    u32 drawable;
};
void sub_0821DE30(void *);
int DisplayAnimation_SetFlags(void *, int);
int DisplayAnimation_ClearFlags(void *, int);
int sub_0802D88C(struct Actor *);
int sub_0802D8BC(struct Actor *);
int sub_0802DB1C(void *unused, struct Actor *p)
{
    int result;
    p->linkId = p->id;
    p->linkPos = p->pos;
    sub_0821DE30(&p->linkId);
    {
        u32 mask = 0x100;
        if (p->flags & mask) {
            DisplayAnimation_SetFlags(&p->drawable, 1);
            result = sub_0802D88C(p);
        } else {
            DisplayAnimation_ClearFlags(&p->drawable, 1);
            {
                u32 mask = 0x200;
                if (p->flags & mask)
                    result = sub_0802D88C(p);
                else
                    result = sub_0802D8BC(p);
            }
        }
    }
    return result;
}
