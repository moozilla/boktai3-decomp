#include "global.h"
struct Entry  {
    u8 bytes[4];
    u8 pad[2];
    u8 flags,kind;
    u32 timer;
};
struct P  {
    u8 pad[0x3c];
    struct Entry entries[2];
};
void sub_080397D4(int,int);
int sub_08038004(struct P*p) {
    struct Entry*a=&p->entries[0],*b=&p->entries[1];
    if(a->kind==0) {
        if(b->kind==1)goto one;
        if(b->kind==2)goto notify;
        goto two;
    }if(a->kind==1) {
        if(b->kind==1) {
            if(a->timer<b->timer)goto zero;
            if(a->timer>b->timer)goto one;
            goto two;
        }if(b->kind!=2)goto zero;
        notify:sub_080397D4(1,1);
        zero:return 0;
    }if(b->kind==1)goto one;
    if(b->kind!=2)goto one;
    sub_080397D4(1,1);
    if(a->timer<b->timer)goto one;
    if(a->timer>b->timer)goto zero;
    two:return 2;
    one:return 1;
}
