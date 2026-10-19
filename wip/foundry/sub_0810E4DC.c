#include "global.h"
#define gPanelKeyState                         gUnk_03005260
#define SelectionPanel_UpdateSelection         sub_0810E4DC
#define SelectionPanel_UpdateConfirmTransition sub_0810E3A0
#define SelectionPanel_UpdateCancelTransition  sub_0810E450
#define SelectionPanel_UpdateImmediateConfirm  sub_0810E378
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetLayerMap(u32, void *, u16);
void Sound_PlaySong(u32);
void sub_0810D688(u8 *);
void sub_0810E1A0(u8 *);
void sub_0810DE88(u8 *);
void sub_0810E12C(u8 *);
u8 sub_0811DAD4(void);
void sub_0811CCE0(u8 *, s32);
void sub_0811D16C(u16 *);
void sub_0811DCB8(s32);
void sub_0816381C(u16 *, u8 *);
void sub_0810E378(u8 *);
void sub_0810E3A0(u8 *);
void sub_0810E450(u8 *);
extern u16 gUnk_03005260[];
extern u32 gUnk_030053F4, gUnk_030053F0;
static inline u8 SelectionPanel_CanConfirm(void)
{
    if (sub_0811DAD4())
        return 1;
    return 0;
}
struct SelectionPanel {
    u8 unk0[0xce4];
    void (*update)(u8 *);
    u8 unkCE8[0x1c];
    u8 result, unkD05, selected, input;
    u8 unkD08[0x24];
    u8 entering;
    u8 unkD2D;
    u16 timer;
};
void sub_0810E4DC(struct SelectionPanel *panel)
{
    u8 *p = (u8 *)panel;
    u8 *flag = &panel->entering;
    if (*flag) {
        Bg_SetLayerMap(3, *(void **)(p + 0x18), 1);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        sub_0810D688(p);
        sub_0811DCB8(3);
        *(u16 *)(p + 0xd2e) = 0;
        p[0xd07] = 0;
        *flag = 0;
    }
    sub_0811CCE0(p + 0xa8c, 0);
    sub_0811D16C((u16 *)(p + 0x470));
    sub_0816381C((u16 *)(p + 0x468), p + 0xd07);
    if (*(u16 *)(p + 0xd2e) <= 29) {
        ++*(u16 *)(p + 0xd2e);
        return;
    }
    {
        u32 keys, selection;
        sub_0810DE88(p);
        sub_0810E12C(p);
        selection = *(u32 *)(p + 0xd00);
        p[0xd06] = selection;
        keys = gUnk_03005260[1];
        if (keys & 1) {
            sub_0810E1A0(p);
            if (SelectionPanel_CanConfirm()) {
                void (*next)(u8 *);
                u32 mask;
                Sound_PlaySong(0xdd);
                mask = 0x800;
                if ((gUnk_030053F4 | gUnk_030053F0) & mask)
                    next = sub_0810E378;
                else
                    next = sub_0810E3A0;
                {
                    u16 one = 1;
                    panel->update = next;
                    panel->entering = one;
                    panel->result = one;
                }
            } else {
                Sound_PlaySong(0x192);
            }
        } else if (keys & 2) {
            u32 mask = 0x800;
            if (!((gUnk_030053F4 | gUnk_030053F0) & mask)) {
                void (*next)(u8 *) = sub_0810E450;
                panel->update = next;
                panel->entering = 1;
                panel->result = 2;
                Sound_PlaySong(0xde);
            }
        }
    }
}
