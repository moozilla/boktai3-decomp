#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct P {
    u8 pad[0x64];
    u32 count,leftCount,rightCount,leftMask,rightMask,extraMask;
};
struct Save {
    u8 pad[0x50];
    s16 leftSelected,rightSelected,extraSelected;
    u8 pad2[0x12];
    s16 leftRows[16];
};
struct ItemInfo {u8 enabled;u8 pad[31];};
extern struct Save *gUnk_02000710;
extern u32 gUnk_03003A50,gUnk_03005308;
extern u16 gUnk_0203B400[];
int Mod(int,int);
void sub_0822D808(int,struct ItemInfo*);
int sub_0822E618(int);
void sub_0822EA8C(int,int);
void sub_0822EACC(int);
void sub_08038170(struct P *input)
{
    struct ItemInfo info;
    int missing,usable;
    register struct P *p asm("r10")=input;
    register int start asm("r4");
    int i;
    register u32 mask asm("r5");
    register int offset asm("r8");
    register int moduloMask asm("r9");
    register int count asm("r1")=p->count;
    int other;
    if(!count||!p->rightCount)return;
    i=0;
    start=count;
    do {
        *(s16*)((u8*)gUnk_02000710+i*2+0x68)=-1;
        i++;
    } while(i<=7);
    start=Mod(gUnk_03003A50,start);
    missing=0;
    usable=0;
    mask=p->leftMask;
    i=0;
    moduloMask=15;
    offset=0;
    do {
        register int n asm("r2")=start;
        register struct Save **slot asm("r12")=&gUnk_02000710;
        register int limit asm("r6")=start+16;
        int index;
        asm("" : "+r"(limit));
        if(start<limit) {
            int item=start&moduloMask;
            u32 bit=1<<item;
            if(mask&bit) {
                *(s16*)((u8*)*slot+offset+0x68)=item;
                mask&=~bit;
                goto selected16;
            }
            while(1) {
                n++;
                if(n>=limit)break;
                item=n&moduloMask;
                bit=1<<item;
                if(mask&bit) {
                    *(s16*)((u8*)*slot+offset+0x68)=item;
                    mask&=~bit;
                    start=n;
                    break;
                }
            }
        }
selected16:
        if(*(s16*)((u8*)gUnk_02000710+offset+0x68)<0)missing=1;
        else {
            sub_0822D808(i,&info);
            if(!info.enabled)usable=1;
        }
        index=(gUnk_03005308+1)&1023;
        gUnk_03005308=index;
        start+=gUnk_0203B400[index]&moduloMask;
        {register int two asm("r0")=2;asm("" : "+r"(two));offset+=two;}
        i++;
    } while(i<=1);
    other=p->rightCount;
    if(missing&&!usable) {
        i=0;
        do {
            s16 *row=(s16*)((u8*)gUnk_02000710+i*2+0x68);
            if(*row<0) {
                *row=0;
                break;
            }
            i++;
        } while(i<=1);
    }
    i=0;
    do {
        *(s16*)((u8*)gUnk_02000710+i*2+0x78)=-1;
        i++;
    } while(i<=7);
    start=Mod(gUnk_03003A50,other);
    missing=0;
    usable=0;
    mask=p->extraMask;
    i=0;
    moduloMask=7;
    offset=0;
    do {
        register int n asm("r2")=start;
        register struct Save **slot asm("r12")=&gUnk_02000710;
        register int limit asm("r6")=start+8;
        int index;
        asm("" : "+r"(limit));
        if(start<limit) {
            int item=start&moduloMask;
            u32 bit=1<<item;
            if(mask&bit) {
                *(s16*)((u8*)*slot+offset+0x78)=item;
                mask&=~bit;
                goto selected8;
            }
            while(1) {
                n++;
                if(n>=limit)break;
                item=n&moduloMask;
                bit=1<<item;
                if(mask&bit) {
                    *(s16*)((u8*)*slot+offset+0x78)=item;
                    mask&=~bit;
                    start=n;
                    break;
                }
            }
        }
selected8:
        {
            s16 *row=(s16*)((u8*)gUnk_02000710+offset+0x78);
            if(*row<0)missing=1;
            else if(sub_0822E618(*row)==1)usable=1;
        }
        index=(gUnk_03005308+1)&1023;
        gUnk_03005308=index;
        start+=gUnk_0203B400[index]&moduloMask;
        {register int two asm("r0")=2;asm("" : "+r"(two));offset+=two;}
        i++;
    } while(i<=1);
    if(missing&&!usable) {
        i=0;
        do {
            s16 *row=(s16*)((u8*)gUnk_02000710+i*2+0x78);
            if(*row<0) {
                *row=0;
                break;
            }
            i++;
        } while(i<=1);
    }
    gUnk_02000710->leftSelected=0;
    sub_0822EA8C(0,0);
    i=1;
    do {
        sub_0822EACC(i);
        i++;
    } while(i<=7);
    gUnk_02000710->extraSelected=0;
}
