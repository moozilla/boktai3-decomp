#include "global.h"
#include "background_controller.h"
#include "render.h"

extern u16 *gUnk_03004278;
extern vu16 *gUnk_03003A00;
extern u16 gUnk_03004270, gUnk_03004274;
u8 *ScanlineEffect_GetBuffer(void);
vu16 *ScanlineEffect_GetHorizontalScrollRegister(s32);
vu16 *ScanlineEffect_GetVerticalScrollRegister(s32);

void ScanlineEffect_StageTransfer(void)
{
    u16 *buffer;
    vu16 *target;
    s32 i;
    u32 scroll;
    if (!gScanlineEffectSource)
        return;
    buffer = (u16 *)ScanlineEffect_GetBuffer();
    target = (vu16 *)0x0400001C;
    switch ((*(s32 *)&gScanlineEffectMode)) {
        case 0:
            target = ScanlineEffect_GetHorizontalScrollRegister((*(s32 *)&gScanlineEffectLayer));
            scroll = (u16)gUnk_03004C30[(*(s32 *)&gScanlineEffectLayer)].x & 0x1FF;
            for (i = 0; i <= 159; i++)
                buffer[i] = (*(u16 *)(i * 2 + (u32)gScanlineEffectSource) + scroll) & 0x1FF;
            break;
        case 1:
            target = ScanlineEffect_GetVerticalScrollRegister((*(s32 *)&gScanlineEffectLayer));
            scroll = (u16)gUnk_03004C30[(*(s32 *)&gScanlineEffectLayer)].y & 0x1FF;
            for (i = 0; i <= 159; i++)
                buffer[i] = (*(u16 *)(i * 2 + (u32)gScanlineEffectSource) + scroll) & 0x1FF;
            break;
        case 2:
            target = (vu16 *)0x0400004C;
            for (i = 0; i <= 159; i++)
                buffer[i] = *(u16 *)(i * 2 + (u32)gScanlineEffectSource);
            break;
        case 3:
            *(vu16 *)0x04000052 = 0x810;
            *(vu16 *)0x04000050 = 0xEDE;
            target = (vu16 *)0x04000054;
            for (i = 0; i <= 159; i++)
                buffer[i] = *(u16 *)(i * 2 + (u32)gScanlineEffectSource) & 31;
            break;
    }
    gUnk_03004278 = buffer;
    gUnk_03003A00 = target;
    gUnk_03004270 = 1;
    gUnk_03004274 = 1;
    gScanlineEffectSource = 0;
}
