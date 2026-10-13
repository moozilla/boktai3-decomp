#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct P {
    u8 pad[0x1e];
    u16 round;
    u8 pad2[4];
    u32 timer;
    u8 state, entered;
    u8 pad3[5];
    u8 mode;
    u8 pad4[4];
    u8 done, flag;
    u8 pad5[14];
    u8 *left;
    u8 pad6[8];
    u8 *right;
    u8 pad7[0x78];
    void *script;
    int window;
    u8 pad8[8];
    u32 reward;
};
struct Global {u8 pad[12];u32 money;};
extern struct Global *gUnk_030053F8;
int sub_08036AA8(void);
void sub_08038158(void);
void sub_0803B700(int,int,void*,void*,u32,u32,void*);
void sub_080380C4(struct P *,int);
int sub_08033690(int,int,int,int);
void sub_0803386C(int,void*);
void sub_08033924(int,int);
void sub_080337FC(int);
void sub_08038E48(struct P *p)
{
    if(p->entered) {
        int offset;
        int side;
        u32 money;
        u8 *left;
        register u8 *right asm("r4");
        p->entered=0;
        p->done=0;
        offset=(p->round-1)*720;
        left=(u8*)(offset+(u32)p->left);
        right=(u8*)(offset+(u32)p->right);
        side=sub_08036AA8();
        money=gUnk_030053F8->money;
        sub_0803B700(side,p->mode,left,right,p->reward,money,sub_08038158);
    }
    if(p->done) {
        int *window=&p->window;
        u32 thirteen=13;
        u8 *state=&p->state;
        u32 zero=0;
        int w;
        *state=thirteen;
        p->entered=1;
        p->timer=zero;
        p->flag=zero;
        sub_080380C4(p,3);
        w=sub_08033690(1,6,28,6);
        *window=w;
        if(w>=0) {
            sub_0803386C(w,p->script);
            sub_08033924(*window,11);
            sub_080337FC(*window);
        }
    }
}
