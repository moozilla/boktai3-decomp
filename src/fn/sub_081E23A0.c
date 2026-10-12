#include "global.h"
struct Sprite {u8 head[8];u32 flags;u8 gap[0x20-12];s16 x,y;u8 tail[0x60-0x24];};
struct Resource {u32 words[8];};
struct Viewer {u8 p0[0x20];struct Sprite background;struct Resource backgroundResource;u8 p1[8];struct Sprite panel;struct Resource panelResource;u8 p2[8];struct Sprite cursor;struct Resource cursorResource;u8 p3[8];struct Sprite label;struct Resource labelResource;u8 p4[8];struct Resource digitsResource;struct Sprite digitsA[3],digitsB[3],labels[3];s32 state,exitFlag,exitTimer,fadeShift,selection,countA,countB,countC,textIndex,valueA,valueB,previousA,previousB,previousC,repeatTimer;const u16 *savedPalette;u16 palette[16];};
void sub_0821980C(void*,void*,u16,s32);s32 Mod(s32,s32);s32 Div(s32,s32);
extern const s16 gUnk_086136F0[],gUnk_086137AE[],gUnk_08613BC4[];
u32 sub_0822B590(s32);void sub_0822B358(s32);void sub_0822B2F8(s32);void sub_08220D78(void*,void*,u16,s32,s32);
extern u16 gUnk_03004FC0[];
void sub_082156B8(s32);void sub_0821656C(s32,s32,s32,s32,s32);void *sub_0821A520(s32,s32);
void sub_082161B4(s32,s32,void*,s32,s32,s32,void*);void sub_082196C4(void*,void*);void sub_0821983C(void*,void*,s32,s32,s32,s32,s32,void*);
void sub_081DC1B4(s32);void sub_081E1AF4(void*);void sub_081E1B80(void*);void sub_081E1BC8(void*);void sub_081E1C10(void*);void sub_081E1C64(void*);void sub_081E1CDC(void*);
struct Vec {s16 x,y,z,w;};
static inline void clear(u32 *value,void *dest){*value=0;CpuSet(value,dest,0x05000002);}
// Load sound-test assets and initialize its category labels, counters and palette.
s32 sub_081E23A0(struct Viewer*p){struct Vec pos;u32 zero;void *asset;s32 option,i;
 clear(&zero,&pos);
 sub_082156B8(0);sub_0821656C(0,0,0,0,0);
 sub_082156B8(2);sub_0821656C(2,0,0,0,0);
 sub_082156B8(3);
 {void *background=sub_0821A520(0xc091,0x3536);option=10;sub_082161B4(2,0,background,0,0,1,&option);}
 sub_0821656C(3,0,0,0,0);
 CpuSet((u8*)sub_0821A520(0x92b3,0x204)+20,gUnk_03004FC0,256);
 asset=sub_0821A520(0xcb05,0xe2ab);
 sub_082196C4(&p->backgroundResource,asset);p->digitsResource=p->backgroundResource;
 sub_0821983C(&p->background,&p->backgroundResource,59,16,0,0,0,0);
 sub_082196C4(&p->labelResource,asset);sub_0821983C(&p->label,&p->labelResource,47,16,0,0,0,0);
 for(i=0;i<3;i++){sub_0821983C(&p->labels[i],&p->digitsResource,43,48,1,0,60,&pos);p->labels[i].flags|=1;pos.y+=32;}
 pos.x=184;pos.y=64;
 for(i=0;i<3;i++){sub_0821983C(&p->digitsA[i],&p->digitsResource,0,48,0,0,60,&pos);p->digitsA[i].flags|=1;pos.x+=8;}
 pos.x=184;pos.y=96;
 for(i=0;i<3;i++){sub_0821983C(&p->digitsB[i],&p->digitsResource,0,48,0,0,60,&pos);p->digitsB[i].flags|=1;pos.x+=8;}
 asset=sub_0821A520(0xcb05,0x530d);
 sub_082196C4(&p->panelResource,asset);sub_0821983C(&p->panel,&p->panelResource,139,16,0,0,0,0);
 sub_082196C4(&p->cursorResource,asset);sub_0821983C(&p->cursor,&p->cursorResource,71,16,0,0,0,0);
 p->savedPalette=*(u16**)((u8*)p+0x178);CpuSet(p->savedPalette,p->palette,16);*(u16**)((u8*)p+0x178)=p->palette;
 p->selection=0;p->fadeShift=3;sub_081DC1B4(3);
 sub_081E1AF4(p);sub_081E1B80(p);sub_081E1BC8(p);sub_081E1C10(p);sub_081E1C64(p);sub_081E1CDC(p);
 return 0;
}
