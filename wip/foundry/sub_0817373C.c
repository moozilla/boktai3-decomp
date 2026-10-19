#define SceneHud_RefreshEquipmentIcons sub_0817373C
#define SceneHud_SetWidgetPosition sub_08165B5C
#define SceneHud_HideWidget sub_08165B7C
#define SceneHud_ShowWidget sub_08165B94
#define SceneHud_SetWidgetPalette sub_08165BF4
#define SceneInventory_IsSlotEquipped sub_0822DF38
#include "global.h"
#include "scene_hud_display.h"
s32 sub_0822D6E0(u32);
s32 sub_0822DCB4(u32);
s32 sub_0822DFDC(u32);
u32 sub_0822DF38(s32);
void sub_08165B5C(void *, s32, s32, s32);
void sub_08165B7C(void *, s32, u32);
void sub_08165B94(void *, s32, u32);
void sub_08165BF4(struct SceneInputWidget *, u16);
void sub_0817373C(struct SceneHudState *p, s32 mode)
{
    s32 i, count;
    s32 j;
    u32 offset;
    struct SceneInputWidget *w;
    for (j=0;j<=7;j++) p->equippedIconSlots[j] |= 0xff;
    count=0;
    i=0;
    w=&p->categoryWidgets[10];
    offset=0;
    for(;i<=15;w++,offset+=0x60,i++) {
        s32 item=sub_0822D6E0(i);
        if(item==0xff) w->flags |= 1;
        else {
            ((struct SceneInputWidget *)((u8 *)p+i*0x60+0x17e0))->flags &= ~1;
            Anim_LoadKeyframe((u8 *)p+(offset+0x17e0),p->motionResource,(u16)(item+0xf0),0);
            if ((mode==0 && !sub_0822DCB4(i)) || (mode==1 && !sub_0822DFDC(i)))
                sub_08165BF4(w,0x374);
            else sub_08165BF4(w,0x373);
            if(sub_0822DF38(i)) {
                s32 index=count+30;
                sub_08165B94(p,index,1);
                sub_08165B5C(p,index,w->position.x,w->position.y);
                p->equippedIconSlots[count]=i;
                count++;
            }
        }
    }
    while(count<=15) {
        sub_08165B7C(p,count+30,1);
        count++;
    }
}
