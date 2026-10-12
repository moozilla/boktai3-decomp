#include "global.h"
struct Object{u8 pad0[0x50];u32 flags;u8 pad54[0xc0];void(*update)(void*);void(*destroy)(void*);u8 pad11c[0x2b];u8 slot;u8 pad148[0xd0];struct Object*next,*prev;};
struct Manager{u8 pad0[0x18];void*targets[4];struct Object*head;u32 ticks,slot,serial;};extern struct Manager*gUnk_020000D8;extern u8*gUnk_02000580[4];int sub_0802C6C8(struct Manager*,struct Object*);void sub_08219D38(void*);void sub_0802C710(struct Manager*);
int sub_0802C684(struct Manager*m,struct Object*p){if(m->head)m->head->next=p;p->next=0;p->prev=m->head;m->head=p;m->serial++;p->slot=m->serial&3;return 0;}
