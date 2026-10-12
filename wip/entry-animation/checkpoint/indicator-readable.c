#include "global.h"
struct Sprite {u8 head[8];u32 flags;u8 gap[0x20-12];s16 x,y;u8 tail[0x60-0x24];};
struct Resource {u32 words[8];};
struct Viewer {u8 p0[0x20];struct Sprite background;struct Resource backgroundResource;u8 p1[8];struct Sprite panel;struct Resource panelResource;u8 p2[8];struct Sprite cursor;struct Resource cursorResource;u8 p3[8];struct Sprite label;struct Resource labelResource;u8 p4[8];struct Resource digitsResource;struct Sprite digitsA[3],digitsB[3],labels[3];s32 state,exitFlag,exitTimer,fadeShift,selection,countA,countB,countC,textIndex,valueA,valueB,previousA,previousB,previousC,repeatTimer;};
void sub_0821980C(void*,void*,u16,s32);s32 Mod(s32,s32);s32 Div(s32,s32);
extern const s16 gUnk_086136F0[],gUnk_086137AE[],gUnk_08613BC4[];
u32 sub_0822B590(s32);void sub_0822B358(s32);void sub_0822B2F8(s32);void sub_08220D78(void*,void*,u16,s32,s32);
s32 sub_08220F70(void*,void*);
// Animate the playing indicator for each sound category.
void sub_081E2094(struct Viewer*p){s32 i=0,active;u32 *flags=&p->labels[0].flags;struct Sprite*label=p->labels;for(;i<3;i++){switch(i){
case 0:active=sub_0822B590(gUnk_086136F0[p->previousA]);break;
case 1:active=sub_0822B590(gUnk_086137AE[p->previousB]);break;
case 2:active=sub_0822B590(gUnk_08613BC4[p->previousC]);break;
default:return;}
 if(active){*flags&=~1;sub_08220F70(label,&p->digitsResource);}
 else if(!(*flags&1)){
 ((u8*)flags)[0x13]=2;
 if(sub_08220F70(label,&p->digitsResource))*flags|=1;
 }
flags=(u32*)((u8*)flags+0x60);label++;
}}
