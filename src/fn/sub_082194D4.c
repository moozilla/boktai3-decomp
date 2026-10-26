#include "global.h"

extern s32 gScanlineEffectLayer, gScanlineEffectMode;
extern u16 *gScanlineEffectSource;
extern u16 *gUnk_03004278;
extern vu16 *gUnk_03003A00;
extern u16 gUnk_03004270, gUnk_03004274;
u8 *ScanlineEffect_GetBuffer(void);
vu16 *ScanlineEffect_GetHorizontalScrollRegister(s32);
vu16 *ScanlineEffect_GetVerticalScrollRegister(s32);
void Memory_Clear(void *, s32);
void Mosaic_ResetParameters(void);

void sub_082194D4(void)
{
    u16 *buffer;
    vu16 *target;
    buffer = (u16 *)ScanlineEffect_GetBuffer();
    Memory_Clear(buffer, 320);
    target = (vu16 *)0x0400001C;
    switch (gScanlineEffectMode) {
        case 0:
            target = ScanlineEffect_GetHorizontalScrollRegister(gScanlineEffectLayer);
            break;
        case 1:
            target = ScanlineEffect_GetVerticalScrollRegister(gScanlineEffectLayer);
            break;
        case 2:
            Mosaic_ResetParameters();
            target = (vu16 *)0x0400004C;
            break;
        case 3:
            target = (vu16 *)0x04000054;
            break;
    }
    gUnk_03004278 = buffer;
    gUnk_03003A00 = target;
    gUnk_03004270 = 1;
    gUnk_03004274 = 0;
    gScanlineEffectSource = 0;
}
