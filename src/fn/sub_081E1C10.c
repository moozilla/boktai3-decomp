#include "global.h"
struct Sprite {u8 head[8];u32 flags;u8 gap[0x20-12];s16 x,y;u8 tail[0x60-0x24];};
struct Resource {u32 words[8];};
struct Viewer {u8 p0[0x20];struct Sprite background;struct Resource backgroundResource;u8 p1[8];struct Sprite panel;struct Resource panelResource;u8 p2[8];struct Sprite cursor;struct Resource cursorResource;u8 p3[8];struct Sprite label;struct Resource labelResource;u8 p4[8];struct Resource digitsResource;struct Sprite digitsA[3],digitsB[3],labels[3];u8 p5[0x5cc-0x5c0];u32 state,selection,countA,countB,countC,textIndex,valueA,valueB;};
void sub_0821980C(void*,void*,u16,s32);s32 Mod(s32,s32);s32 Div(s32,s32);
extern u8 *gUnk_020003C0;
void sub_082156B8(s32);void sub_0821656C(s32,s32,s32,s32,s32);
s32 Script_ParseStringRef(void*);const char*Text_LookupString(s32);void Menu_EraseRect(s32,s32,s32,s32);void Menu_DrawText(s32,s32,const char*);
// Refresh the selected collection entry's text label.
void sub_081E1C10(struct Viewer*p){const char*text;
 sub_082156B8(0);sub_0821656C(0,0,0,0,0);
 text=Text_LookupString(Script_ParseStringRef(gUnk_020003C0)+p->textIndex);
 Menu_EraseRect(10,4,16,2);Menu_DrawText(10,4,text);
}
