#include "global.h"
#define SoundTest_Destroy sub_081E2300
// Provisional name: SoundTest_Destroy
struct Sprite {u8 head[8];u32 flags;u8 gap[0x20-12];s16 x,y;u8 tail[0x60-0x24];};
struct Resource {u32 words[8];};
struct Viewer {u8 p0[0x20];struct Sprite background;struct Resource backgroundResource;u8 p1[8];struct Sprite panel;struct Resource panelResource;u8 p2[8];struct Sprite cursor;struct Resource cursorResource;u8 p3[8];struct Sprite label;struct Resource labelResource;u8 p4[8];struct Resource digitsResource;struct Sprite digitsA[3],digitsB[3],labels[3];s32 state,exitFlag,exitTimer,fadeShift,selection,countA,countB,countC,textIndex,valueA,valueB,previousA,previousB,previousC,repeatTimer;};
void sub_0821980C(void*,void*,u16,s32);s32 Mod(s32,s32);s32 Div(s32,s32);
extern const s16 gUnk_086136F0[],gUnk_086137AE[],gUnk_08613BC4[];
u32 sub_0822B590(s32);void sub_0822B358(s32);void sub_0822B2F8(s32);void sub_08220D78(void*,void*,u16,s32,s32);
extern u16 gUnk_03004BD8;void sub_082195E0(void*);
// Release every sprite used by the sound-test menu.
void SoundTest_Destroy(struct Viewer*p){s32 i;struct Sprite*a,*b,*c;
 {s32 mask=3;gUnk_03004BD8=~mask&gUnk_03004BD8;}
 p->cursor.flags|=1;p->panel.flags|=1;p->background.flags|=1;
 c=p->digitsB;b=p->digitsA;a=p->labels;
 for(i=2;i>=0;i--){sub_082195E0(a);sub_082195E0(b);sub_082195E0(c);c++;b++;a++;}
 sub_082195E0(&p->cursor);sub_082195E0(&p->panel);sub_082195E0(&p->label);sub_082195E0(&p->background);
}
