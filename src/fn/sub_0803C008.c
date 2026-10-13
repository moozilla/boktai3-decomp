#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct P {u8 pad[0x24];u32 mask[8];};
struct Slot {
    u8 pad[0x56c];
    u32 flags;
    u8 pad2[0x2e0];
    u8 selector;
    u8 pad3[3];
    s32 id;
};
struct Item {u8 pad[0x10];u32 id;};
int Mod(int,int);
void sub_0811CD24(void*,int);
void sub_08112C54(int);
void sub_0811EE0C(void*);
void sub_0803C008(struct P *input,struct Slot *slotInput,struct Item *item)
{
    register struct P *p asm("r5")=input;
    register struct Slot *slot asm("r6")=slotInput;
    u32 id=item->id;
    int shift=Mod(id,32);
    register u32 index asm("r4")=(id>>5)*4;
    u32 *mask;
    u32 one;
    asm("" : "+r"(index));
    mask=p->mask;
    mask=(u32*)((u8*)mask+index);
    one=1;
    *mask&=~(one<<shift);
    sub_0811CD24((u8*)slot+0x600,1);
    sub_08112C54((s8)slot->selector);
    {
        u32 *flags=(u32*)((u8*)slot+0x56c);
        *flags|=one;
    }
    sub_0811EE0C(slot);
    {
        register u32 offset asm("r1")=0x854;
        asm("" : "+r"(offset));
        slot=(struct Slot*)((u8*)slot+offset);
        *(s32*)slot=-1;
    }
}
