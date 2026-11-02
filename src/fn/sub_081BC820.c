#include "scene_input.h"
void Bg_SetLayerMap(u32, u32, u32);
struct StatGaugeState;
void DarkLoan_HideSprites(struct StatGaugeState *);
void DarkLoan_UpdateBalanceDigits(struct StatGaugeState *);
void StatGauge_DrawPreview(struct StatGaugeState *);
void DarkLoan_UpdateAdjustmentDigits(struct StatGaugeState *);
void MessageWindow_ShowMessage(u32);
void DarkLoan_UpdateAdjustment(void);
void DarkLoan_SetState(struct StatGaugeState *, void (*)(void));
struct StatGaugeState {
    u8 pad0[0x18];
    struct SceneInputResource resource, secondaryResource;
    struct SceneInputWidget widgets[22];
    u32 extraFlags;
    u8 pad89C[0x28];
    u8 palette[0x1C];
    u32 layerMap;
    u8 pad8E4[0xA];
    u16 adjustment;
    u8 pad8F0[8];
    u16 state[10];
};

static inline void ShowWidget(struct SceneInputWidget *widget)
{
    widget->flags &= ~1;
}
void StatGauge_BeginAdjustment(struct StatGaugeState *p)
{
    s32 i;
    Bg_SetLayerMap(0, p->layerMap, 4);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    DarkLoan_HideSprites((u8 *)p);
    p->widgets[0].flags &= ~1;
    Anim_SetKeyframes(&p->widgets[0], &p->resource, 5, 1, 0);
    p->widgets[5].flags &= ~1;
    Anim_LoadKeyframe(&p->widgets[5], &p->resource, 18, 1);
    p->adjustment = 0;
    for (i = 0; i < 4; i++) {
        p->widgets[6 + i].flags &= ~1;
    }
    DarkLoan_UpdateBalanceDigits(p);
    StatGauge_DrawPreview(p);
    p->widgets[14].flags &= ~1;
    for (i = 0; i < 4; i++) {
        p->widgets[10 + i].flags &= ~1;
        p->widgets[15 + i].flags &= ~1;
    }
    DarkLoan_UpdateAdjustmentDigits(p);
    p->state[8] = 0;
    p->state[9] = 0;
    ShowWidget(&p->widgets[19]);
    p->extraFlags &= ~1;
    AnimData_SelectPaletteBank(p->palette, 0x1B4);
    ShowWidget(&p->widgets[20]);
    ShowWidget(&p->widgets[21]);
    MessageWindow_ShowMessage(4);
    p->state[1] = 0;
    p->state[2] = 0;
    p->state[3] = 0;
    p->state[4] = 0;
    p->state[0] = 0;
    DarkLoan_SetState(p, DarkLoan_UpdateAdjustment);
}
