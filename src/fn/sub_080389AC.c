#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct P {
    u8 pad[0x1e];
    u16 rounds;
    u8 pad2[4];
    u32 timer;
    u8 state, entered;
    u8 pad3[0xc2];
    u8 mode;
};
struct Save {u8 pad[0x52];s16 selected;};
extern struct Save *gUnk_02000710;
extern u32 gUnk_0300523C;
void sub_08038128(struct P *,int);
void sub_08038138(struct P *);
void sub_08158FE8(void);
void sub_08158FC0(void);
void sub_080398BC(int);
void sub_08159010(void);
static inline int item(void)
{
    struct Save *s=gUnk_02000710;
    return *(s16*)((u8*)s+s->selected*2+0x88);
}
void sub_080389AC(struct P *p)
{
    if(p->entered) {
        p->entered=0;
        p->rounds++;
        if(p->rounds>98)p->rounds=99;
        sub_08038128(p,3);
    }
    if(p->timer>89) {
        u32 seven=7;
        u8 *state=&p->state;
        u32 zero=0;
        u32 one;
        u8 *entered;
        register u8 *mode asm("r2");
        *state=seven;
        entered=&p->entered;
        one=1;
        *entered=one;
        p->timer=zero;
        {register u32 *global asm("r2")=&gUnk_0300523C;
        register u32 mask asm("r1")=~4;
        *global &= mask;}
        sub_08038138(p);
        mode=&p->mode;
        switch(*mode) {
        case 0: {
            int n=item();
            if(n==1)goto mode2;
            if(n!=2)goto done;
            goto mode1;
        }
        case 2:
            if(item()==2) {
            mode1:
                *mode=one;
                sub_08158FE8();
            } else {
                *mode=zero;
                sub_08158FC0();
            }
            break;
        case 1:
            if(item()==1) {
            mode2:
                *mode=2;
                sub_08158FC0();
            } else {
                *mode=zero;
                sub_08158FE8();
            }
            break;
        }
        done:
        sub_080398BC(item());
        sub_08159010();
    } else p->timer++;
}
