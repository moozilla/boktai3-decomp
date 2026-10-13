#include "global.h"
struct Entry {u8 bytes[4];u8 pad[2];u8 flags,kind;u32 timer;};
struct P {u8 pad[0x3c];struct Entry entries[1];};
void sub_08038074(struct P*p,int index){struct Entry*e=&p->entries[index];u8*q;u32 z;e->flags=0;z=0;q=&e->bytes[3];do{*q=z;q--;}while((s32)q>=(s32)e->bytes);}
