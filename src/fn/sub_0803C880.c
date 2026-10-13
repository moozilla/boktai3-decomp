#include "global.h"
struct Slot {u8 pad[0x854];s32 id;u8 pad2[0x58];};
struct Item {u8 bytes[380];};
struct P {
    u8 pad[0x20];
    struct Item *items;
    u8 pad2[0x20];
    struct Slot slots[4];
    u8 pad3[0x40];
    u32 count,active;
};
int sub_0803C7EC(struct P*,struct Slot*,struct Item*);
void sub_0803C008(struct P*,struct Slot*,struct Item*,u32);
void sub_0803C114(struct P*,struct Slot*,struct Item*,u32);
void sub_0803C880(struct P *p)
{
    struct Slot *slot;
    u32 i;
    p->active=0;
    slot=p->slots;
    i=0;
    while(i<p->count) {
        if(slot->id!=-1) {
            struct Item *item=&p->items[slot->id];
            if(sub_0803C7EC(p,slot,item))sub_0803C008(p,slot,item,i);
            else sub_0803C114(p,slot,item,i);
            p->active++;
        }
        i++;
        slot++;
    }
}
u32 sub_0803C90C(u32 unused,u32 value)
{
    return (value>>16)&15;
}
