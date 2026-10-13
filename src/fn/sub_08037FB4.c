#include "global.h"
struct Packet {
    u8 pad,type;
    u8 pad2[2];
    u32 words[1];
};
struct P {
    u8 pad[0x2a];
    u8 busy;
    u8 pad1[0x29];
    struct Packet packet;
};
int sub_08036B64(struct Packet*);
int sub_08037FB4(struct P*p,int type,const u32*src,int count) {
    struct Packet*q=&p->packet;
    u32*dst;
    int r;
    if(p->busy&&q->type)return -1;
    q->type=type;
    if(count>0) {
        dst=q->words;
        do {
            *dst++=*src++;
            count--;
        }while(count);
    }r=sub_08036B64(q);
    if(r<0&&q->type)p->busy=1;
    return r;
}
