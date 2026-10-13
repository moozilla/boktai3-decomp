#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Entry  {
    u8 bytes[4],a,b,flag,kind;
    u32 timer;
};
struct P  {
    u8 pad[0x1c];
    u16 remaining;
    u8 pad2[2];
    u32 flags,timer;
    u8 state,entered;
    u8 pad3[2];
    u32 status;
    u8 pad4[7];
    u8 active;
    u8 scores[4];
    struct Entry entry;
};
struct Actor  {
    u8 pad[0x424];
    u16 current,maximum;
};
struct Target  {
    u8 pad[0x94];
    s16 current,maximum;
};
extern struct Actor *gUnk_02000580;
extern u32 gUnk_03005308,gUnk_0300523C;
extern u16 gUnk_0203B400[];
void sub_081783C0(u16*);
void sub_08038128(struct P*,int);
void sub_0822AE7C(int);
void sub_08038148(struct P*);
void sub_08038138(struct P*);
int Mod(int,int);
int Div(int,int);
struct Target *sub_0802140C(int);
void sub_080397D4(int,int);
int sub_08037FB4(struct P*,int,const u32*,int);
void sub_08178404(void);
void sub_0822B2F8(int);
static inline u32 test(u32 *flags,u32 mask)  {
    return *flags&mask;
}
static inline void set(u32 mask)  {
    gUnk_0300523C|=mask;
}
void sub_08038AD4(struct P *input)
{
    register struct P *p asm("r8")=input;
    u32 data;
    register u8 *entered asm("r1")=&p->entered;
    if(*entered)  {
        *entered=0;
        {
            register u32 value asm("r0")=720;
            register struct P *view asm("r1");
            asm("" : "+r"(value));
            view=p;
            view->remaining=value;
        }
        sub_081783C0(&p->remaining);
        sub_08038128(p,4);
        sub_0822AE7C(22);
        p->active=1;
    }
    sub_08038148(p);
    if(( {
        register struct P *view asm("r2")=p;
        asm("" : "+r"(view));
        view->timer;
    })==30)sub_08038138(p);
    if(Mod(( {
        register struct P *view asm("r3")=p;
        asm("" : "+r"(view));
        view->timer;
    }),60)==0)  {
        struct Actor *actor=gUnk_02000580;
        struct Target *target=sub_0802140C(0);
        int a,b;
        register struct Entry *e asm("r6");
        register u32 packed asm("r10");
        register int total asm("r9");
        int i;
        register int shift asm("r12");
        register u8 *q asm("r5");
        if(actor&&actor->maximum)a=Div(100*actor->current,actor->maximum);
        else a=0;
        if(a==0&&actor->current)a=1;
        if(target&&target->maximum>0)b=Div(100*target->current,target->maximum);
        else b=0;
        if(b==0&&target->current>0)b=1;
        data=a|(b<<8);
        e=&p->entry;
        asm("" : "+r"(e));
        {
            register u32 zero asm("r3")=0;
            asm("" : "+r"(zero));
            packed=zero;
            total=zero;
        }
        i=0;
        {
            register u32 zero asm("r0")=0;
            asm("" : "+r"(zero));
            shift=zero;
        }
        q=e->bytes;
        do  {
            int value;
            register u32 flag asm("r4")=e->flag;
            if(flag)value=*q;
            else  {
                register int old asm("r0")=*q;
                register u8 *score asm("r1");
                register int n asm("r2");
                asm("" : "+r"(old));
                score=p->scores;
                asm("" : "+r"(score));
                score+=i;
                n=*score;
                value=old+n;
                total+=n;
                *score=flag;
            }
            e->bytes[i]=0;
            if(value>15)value=15;
            {
                register int count asm("r1")=shift;
                register u32 merged asm("r2");
                asm("" : "+r"(count));
                value<<=count;
                merged=packed;
                asm("" : "+r"(merged));
                merged|=value;
                packed=merged;
            }
            q++;
            i++;
            shift+=4;
        } while(i<=3);
        if(( {
            register int v asm("r0")=total;
            asm("" : "+r"(v));
            v;
        })>0)sub_080397D4(3,1);
        {
            register u32 v asm("r2")=packed;
            register u32 shifted asm("r1");
            asm("" : "+r"(v));
            shifted=v<<16;
            asm("" : "+r"(shifted));
            data|=shifted;
        }
        sub_08037FB4(p,0,&data,1);
    }
    {
        register struct P *view asm("r3")=p;
        asm("" : "+r"(view));
        if((view->status&0xffff00)||test(&view->flags,128))  {
            u32 eight;
            u8 *state;
            u32 zero;
            sub_08178404();
            eight=8;
            state=&p->state;
            zero=0;
            *state=eight;
            p->entered=1;
            p->timer=zero;
            return;
        }
    }
    {
        register struct P *view asm("r1")=p;
        u32 remaining;
        asm("" : "+r"(view));
        remaining=--view->remaining;
        if(!remaining)  {
            u32 two;
            u8 *state;
            sub_08178404();
            sub_0822B2F8(0x372);
            two=2;
            state=&p->state;
            *state=two;
            p->entered=1;
            {
                register struct P *view asm("r2")=p;
                asm("" : "+r"(view));
                view->timer=remaining;
            }
            set(4);
            p->active=0;
        } else  {
            register struct P *view asm("r3")=p;
            asm("" : "+r"(view));
            view->timer++;
        }
    }
}
