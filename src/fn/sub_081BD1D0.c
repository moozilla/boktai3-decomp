#define StatGauge_CloseAfterDelay sub_081BD1D0
#define StatGauge_SetIdleCallback DarkLoan_HideDisplay
#define gScreenFlags gUnk_03004BD8
#include "global.h"
struct DirectionalScriptArgs;
void Script_Call(u32, struct DirectionalScriptArgs *);
void Task_Kill(void *);
void DarkLoan_HideDisplay(void *);
void sub_082157EC(void);
extern u16 gUnk_03004BD8;
struct StatGaugePreview {
    u8 unknown0[0x8f4];
    u32 exitScript;
    u8 unknown8f8[0xc];
    u16 timer;
};
void sub_081BD1D0(struct StatGaugePreview *p)
{
    if (++p->timer > 31) {
        if (p->exitScript == 0) {
            u16 *flags = &gScreenFlags;
            u32 mask = ~0x100;
            *flags = mask & *flags;
            DarkLoan_HideDisplay(p);
        } else {
            Script_Call(p->exitScript, 0);
            Task_Kill(p);
            sub_082157EC();
            {
                u16 *flags = &gScreenFlags;
                u32 mask = ~0x100;
                *flags = mask & *flags;
            }
        }
    }
}
