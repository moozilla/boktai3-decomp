#define PauseMenu_UpdateConfigChoice sub_08175D78
#define PauseMenu_SetCallback Hud_SetInteractionCallback
#define gPauseMenuKeys gUnk_03005260
#include "global.h"
struct PauseMenuConfig {
    u8 unknown0[0x64];
    u8 animationData[0x159C];
    u8 animation[4];
};
extern u16 gUnk_03005260[];
void Sound_PlaySong(u32);
s32 Anim_Step(void *, void *);
void sub_081758B4(struct PauseMenuConfig *);
void sub_08175934(struct PauseMenuConfig *);
void sub_0817597C(struct PauseMenuConfig *, u32);
void sub_08175854(u32);
void Hud_SetInteractionCallback(struct PauseMenuConfig *, void (*)(void), u32);
void sub_081660D0(struct PauseMenuConfig *, u32, s32);
void sub_08175BA8(struct PauseMenuConfig *);
s32 sub_08175C40(struct PauseMenuConfig *);
void sub_08175E24(void);
void sub_08175D34(void);
void sub_08175D78(struct PauseMenuConfig *s)
{
    if (gUnk_03005260[1] & 1) {
        Sound_PlaySong(0xDD);
        sub_081758B4(s);
        sub_0817597C(s, 0);
        sub_08175854(7);
        Hud_SetInteractionCallback(s, sub_08175E24, 1);
    } else if (gUnk_03005260[1] & 2) {
        Sound_PlaySong(0xDE);
        sub_08175934(s);
        sub_0817597C(s, 0);
        sub_08175854(5);
        sub_081660D0(s, 1, -1);
        sub_08175BA8(s);
        Hud_SetInteractionCallback(s, sub_08175D34, 0);
    } else {
        if (sub_08175C40(s)) sub_0817597C(s, 1);
        Anim_Step(s->animation, s->animationData);
    }
}
