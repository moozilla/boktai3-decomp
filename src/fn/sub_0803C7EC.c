#include "global.h"
struct P {
    u8 pad[0x2304];
    u32 expected;
    int count;
    u8 pad2[8];
    u32 edges[8];
};
struct Item {
    u8 pad[0x14];
    u32 position;
    u8 pad2[14];
    u16 flags;
    u8 pad3[12];
    u32 threshold;
};
int sub_0803C7EC(struct P *p,void *unused,struct Item *item)
{
    int bits=0;
    int i=0;
    if(bits<p->count) {
        int count=p->count;
        u32 *edge=p->edges;
        do {
            u32 value=*edge;
            if(value>0x18ffff) {
                u32 one=1;
                u32 flag=item->flags&one;
                u32 position=item->position;
                if(flag&&position<item->threshold)goto next;
                if(position<=value-0x190000 || position>=edge[2]+0x190000)bits|=one<<i;
            }
        next:
            edge++;
            i++;
        } while(i<count);
    }
    if(bits==p->expected)return 1;
    return 0;
}
