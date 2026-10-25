#include "global.h"
struct AnimBox {
    u8 pad0[8];
    u32 flags;
};
struct OptionData {
    u8 pad0[0x428];
    u16 a428;
    u16 a42A;
};
struct OptionHud {
    u8 pad0[0xA40];
    struct OptionData *data;
    u8 pad_a44[0x15];
    u8 flagA59;
    u8 pad_a5a[0x42];
    u8 anim[0x10];
    u16 pad_aac[0];
};
extern struct OptionHud *gUnk_02000214;
void Hud_LoadConditionalPaletteRows(void *);
void sub_08168850(void *);
void SceneHud_UpdateSecondaryWidgets(void *);
void SceneHud_RebuildPrimaryOptions(void *);
void PauseMenu_RefreshEquipmentSlotIcons(void *);
void sub_08168214(void *);
u32 WirelessGame_IsItemToggleEnabled(void);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void sub_08164480(void *, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);
void Hud_SetUpdateCallback(void *, void (*)(void));
void SceneHud_UpdateOptionControls(void);
void SceneHud_EnterOptionControls(void)
{
    struct OptionHud *p = gUnk_02000214;
    if (p) {
        Hud_LoadConditionalPaletteRows(p);
        sub_08168850(p);
        SceneHud_UpdateSecondaryWidgets(p);
        SceneHud_RebuildPrimaryOptions(p);
        PauseMenu_RefreshEquipmentSlotIcons(p);
        sub_08168214(p);
        p->flagA59 = 0;
        if (WirelessGame_IsItemToggleEnabled()) {
            struct AnimBox *box = (struct AnimBox *)((u8 *)p + 0xA9C);
            box->flags &= ~1;
            Anim_LoadKeyframe(box, (u8 *)p + 0x144, 0x1C, 1);
        }
        p->data->a428 = p->data->a42A;
        sub_08164480(p, 0, 4, 2, p->data->a428, p->data->a42A, 1);
        Sound_PlaySong(0x10C);
        *(u16 *)((u8 *)p + 0xA80) = 0;
        Hud_SetUpdateCallback(p, SceneHud_UpdateOptionControls);
    }
}
