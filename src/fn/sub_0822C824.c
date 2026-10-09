#include "global.h"
struct Obj { u8 pad[0x19];u8 state;u16 pad1a;s16 a,b;u16 count; };
struct Context { u8 pad[0x874];s16 a,b; };
extern u8 *gUnk_02000710;extern u16 gUnk_030054C0,gUnk_030054C4;
void sub_08243E4C(void),sub_0822C598(struct Obj*);
s32 sub_0822C388(void),sub_0822C238(s32),sub_0822C2B0(s32);
void sub_0822C824(struct Obj *obj) {
 switch(obj->state) {
 case 0:if(++obj->count>29) {sub_08243E4C();obj->state=1;obj->count=0;}break;
 case 1:if(++obj->count>59) {obj->state=2;obj->count=0;}break;
 case 2:
 obj->a=sub_0822C388();
 obj->b=sub_0822C238(obj->a);
 (*(s16*)(gUnk_02000710+0x874))=sub_0822C2B0(obj->a);
 (*(s16*)(gUnk_02000710+0x876))=sub_0822C238((*(s16*)(gUnk_02000710+0x874)));
 sub_0822C598(obj);
 gUnk_030054C0=(*(s16*)(gUnk_02000710+0x874));
 gUnk_030054C4=(*(s16*)(gUnk_02000710+0x876));
 break;
 }
}
