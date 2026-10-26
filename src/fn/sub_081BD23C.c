#define gScreenFlags gDispCntShadow
#include "global.h"
#include "render.h"
struct StatGaugePreview {
    u8 unknown0[0x18];
    u8 backgroundData[0x40];
    u8 backgroundAnim[0x60];
    u8 unknownB8[0x83c];
    u32 exitScript;
    u8 unknown8F8[0xc];
    u16 timer;
};
struct DirectionalScriptArgs;
s32 Anim_Step(void *, void *);
void DarkLoan_HideDisplay(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Script_Call(u32, struct DirectionalScriptArgs *);
void Task_Kill(void *);
void StatGauge_UpdateClosing(struct StatGaugePreview *p)
{
    Anim_Step(p->backgroundAnim, p->backgroundData);
    if (p->timer == 150)
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
    if (++p->timer > 181) {
        if (p->exitScript == 0) {
            u16 *flags = &gScreenFlags;
            u32 mask = ~0x100;
            *flags = mask & *flags;
            DarkLoan_HideDisplay(p);
        } else {
            Script_Call(p->exitScript, 0);
            Task_Kill(p);
        }
    }
}
