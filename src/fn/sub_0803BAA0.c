#include "global.h"
struct Slot {
    u8 pad[0x854];
    s32 id;
    u8 pad2[0x58];
};
struct P {
    u8 pad[0x44];
    struct Slot slots[4];
    u8 pad2[0x40];
    u32 count;
};
struct Slot *sub_0803BAA0(struct P *p)
{
    struct Slot *slot=p->slots;
    u32 i=0;
    u32 total=p->count;
    if(i<total) {
        u32 offset=0x854;
        s32 empty=-1;
        u32 stride=0x8b0;
        u32 count;
        count=total;
        do {
            if(*(s32*)((u8*)slot+offset)==empty)return slot;
            i++;
            slot=(struct Slot*)((u8*)slot+stride);
        } while(i<count);
    }
    return 0;
}
void sub_0811BD30(void*,void*);
void sub_0803BAE4(u8 *p)
{
    sub_0811BD30(p,p+0x600);
}
struct TailData {
    u8 pad[12];
    u16 timer;
    u8 flag;
};
struct Tail {
    u8 pad[0x860];
    struct TailData data;
};
void sub_0803BAF4(struct Tail *p)
{
    struct TailData *data=&p->data;
    data->flag=0;
    data->timer=0;
}
