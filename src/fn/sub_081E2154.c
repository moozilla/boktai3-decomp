#include "global.h"
struct Sprite {u8 head[8];u32 flags;u8 gap[0x20-12];s16 x,y;u8 tail[0x60-0x24];};
struct Resource {u32 words[8];};
struct Viewer {u8 p0[0x20];struct Sprite background;struct Resource backgroundResource;u8 p1[8];struct Sprite panel;struct Resource panelResource;u8 p2[8];struct Sprite cursor;struct Resource cursorResource;u8 p3[8];struct Sprite label;struct Resource labelResource;u8 p4[8];struct Resource digitsResource;struct Sprite digitsA[3],digitsB[3],labels[3];s32 state,exitFlag,exitTimer,fadeShift,selection,countA,countB,countC,textIndex,valueA,valueB,previousA,previousB,previousC,repeatTimer;};
void sub_0821980C(void*,void*,u16,s32);s32 Mod(s32,s32);s32 Div(s32,s32);
extern const s16 gUnk_086136F0[],gUnk_086137AE[],gUnk_08613BC4[];
u32 sub_0822B590(s32);void sub_0822B358(s32);void sub_0822B2F8(s32);void sub_08220D78(void*,void*,u16,s32,s32);
extern u16 gUnk_03005260[];extern void *gUnk_020003C0[];
void sub_081DC1DC(s32);s32 sub_081E1E50(void*);void sub_081E2094(void*);void sub_081E1D50(void*);void sub_081E1BC8(void*);void sub_081E1F5C(void*);void sub_081E1FF8(void*);void sub_0816381C(void*,void*);void sub_0821AD08(void*,void*);void sub_0821A0C0(void*);
// Handle sound-test controls, accelerating held left/right, and the exit fade.
void sub_081E2154(struct Viewer*p){s32 direction,interval;
 switch(p->state){
 case 0:
 if((gUnk_03005260[1]&2)&&sub_081E1E50(p)){
 sub_081DC1DC(p->fadeShift);p->state=1;p->exitFlag=0;sub_0822B2F8(222);break;
 }
 sub_081E2094(p);
 if(gUnk_03005260[1]&1)sub_081E1D50(p);
 if(gUnk_03005260[1]&64){if(--p->selection<0)p->selection=2;sub_081E1BC8(p);sub_0822B2F8(0x37c);}
 else if(gUnk_03005260[1]&128){if(++p->selection>2)p->selection=0;sub_081E1BC8(p);sub_0822B2F8(0x37c);}
 direction=0;interval=5;
 if(gUnk_03005260[1]&32)direction=1;
 else if(gUnk_03005260[1]&16)direction=2;
 else if(gUnk_03005260[0]&48){
 if(++p->repeatTimer>14){if(p->repeatTimer>199)interval=2;if(Mod(p->repeatTimer,interval)==0){if(gUnk_03005260[0]&32)direction=1;else direction=2;}}
 }else p->repeatTimer=0;
 if(direction==1)sub_081E1F5C(p);else if(direction==2)sub_081E1FF8(p);
 sub_0816381C((u8*)p+0x618,(u8*)p+0x620);break;
 case 1:
 if(p->exitTimer>(1<<p->fadeShift)){if(!p->exitFlag)sub_0821AD08(gUnk_020003C0[1],0);sub_0821A0C0(p);}
 p->exitTimer++;break;
 case 2:break;
 }
}
