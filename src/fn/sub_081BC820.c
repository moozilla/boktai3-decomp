// MATCHING_CONCESSION: register-pinning
#include "global.h"
void Bg_SetLayerMap(u32, u32, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void DarkLoan_HideSprites(u8 *);
void Anim_SetKeyframes(u8 *, u8 *, u32, u32, u32);
void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);
void DarkLoan_UpdateBalanceDigits(u8 *);
void StatGauge_DrawPreview(u8 *);
void DarkLoan_UpdateAdjustmentDigits(u8 *);
void AnimData_SelectPaletteBank(u8 *, u32);
void MessageWindow_ShowMessage(u32);
void DarkLoan_UpdateAdjustment(void);
void DarkLoan_SetState(u8 *, void (*)(void));
struct StatGaugeWidget {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x54];
};
struct StatGaugeState {
    u8 pad0[0x18];
    u8 resource[0x40];
    struct StatGaugeWidget widgets[22];
    u32 extraFlags;
    u8 pad89C[0x28];
    u8 palette[0x1C];
    u32 layerMap;
    u8 pad8E4[0xA];
    u16 adjustment;
    u8 pad8F0[8];
    u16 state[10];
};

void StatGauge_BeginAdjustment(u8 *context)
{
    register u8 *s;
    register s32 count;
    u32 latezero;
    register u32 mask asm("r4");
    register u32 mask2 asm("r1");
    s8 mask3;
    u32 value;
    register u8 *resource asm("r6");
    u32 *p;
    register u32 *q;
    u32 *initial;
    register u32 *c1 asm("r2");
    s = context;
    Bg_SetLayerMap(0, ((struct StatGaugeState *)s)->layerMap, 4);
    count = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, count);
    DarkLoan_HideSprites(s);
    value = ((struct StatGaugeState *)s)->widgets[0].flags;
    value &= (mask = ~1);
    ((struct StatGaugeState *)s)->widgets[0].flags = value;
    Anim_SetKeyframes((u8 *)&((struct StatGaugeState *)s)->widgets[0],
                      resource = ((struct StatGaugeState *)s)->resource, 5, 1, count);
    ((struct StatGaugeState *)s)->widgets[5].flags &= mask;
    Anim_LoadKeyframe((u8 *)&((struct StatGaugeState *)s)->widgets[5], resource, 0x12, 1);
    ((struct StatGaugeState *)s)->adjustment = count;
    /* Continue from the resource block to the first widget's flags. */
    resource += 0x48;
    initial = &((struct StatGaugeState *)s)->widgets[6].flags;
    count = 3;
    do {
        *initial &= mask;
        initial = (u32 *)(((u8 *)initial) + 0x60);
    } while ((--count) >= 0);
    DarkLoan_UpdateBalanceDigits(s);
    StatGauge_DrawPreview(s);
    {
        u32 flags = ((struct StatGaugeState *)s)->widgets[14].flags;
        flags &= (mask2 = ~1);
        ((struct StatGaugeState *)s)->widgets[14].flags = flags;
    }
    q = &((struct StatGaugeState *)resource)->widgets[14].flags;
    p = &((struct StatGaugeState *)resource)->widgets[9].flags;
    c1 = p;
    count = 3;
    do {
        *c1 &= mask2;
        *q &= mask2;
        q = (u32 *)(((u8 *)q) + 0x60);
        c1 = (u32 *)(((u8 *)c1) + 0x60);
    } while ((--count) >= 0);
    latezero = 0;
    DarkLoan_UpdateAdjustmentDigits(s);
    ((struct StatGaugeState *)s)->state[8] = latezero;
    ((struct StatGaugeState *)s)->state[9] = latezero;
    {
        struct StatGaugeWidget *widget = &((struct StatGaugeState *)s)->widgets[19];
        widget->flags &= (mask3 = ~1);
    }
    ((struct StatGaugeState *)s)->extraFlags &= mask3;
    AnimData_SelectPaletteBank(((struct StatGaugeState *)s)->palette, 0x1B4);
    {
        struct StatGaugeWidget *widget = &((struct StatGaugeState *)s)->widgets[20];
        widget->flags &= mask3;
    }
    {
        struct StatGaugeWidget *widget = &((struct StatGaugeState *)s)->widgets[21];
        widget->flags &= mask3;
    }
    MessageWindow_ShowMessage(4);
    ((struct StatGaugeState *)s)->state[1] = latezero;
    ((struct StatGaugeState *)s)->state[2] = latezero;
    ((struct StatGaugeState *)s)->state[3] = latezero;
    ((struct StatGaugeState *)s)->state[4] = latezero;
    ((struct StatGaugeState *)s)->state[0] = latezero;
    DarkLoan_SetState(s, DarkLoan_UpdateAdjustment);
}
