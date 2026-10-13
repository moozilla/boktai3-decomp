#include "global.h"
struct Entry  {
    u8 bytes[4];
    u8 pad[2];
    u8 flags,kind;
    u32 timer;
};
struct P  {
    u8 pad[0x1c];
    u16 time;
    u8 pad1[15];
    u8 a,b;
    u8 pad2[13];
    struct Entry entry;
};
void sub_08038094(struct P*p) {
    struct Entry*e=&p->entry;
    u32 v=p->a;
    if(v)v=2;
    else  {
        v=p->b;
        if(v)v=1;
    }e->kind=v;
    e->timer=720-p->time;
}
