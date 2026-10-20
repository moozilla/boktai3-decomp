#include "global.h"
#include "background_controller.h"

#define ScanlineEffect_StageTransfer sub_082192F0
extern s32 gUnk_03005220, gUnk_03005228;
extern u16 *gUnk_03005224;
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
    if (!gUnk_03005224)
        return;
    buffer = (u16 *)ScanlineEffect_GetBuffer();
    target = (vu16 *)0x0400001C;
    switch (gUnk_03005228) {
    case 0:
        target = ScanlineEffect_GetHorizontalScrollRegister(gUnk_03005220);
        scroll = (u16)gUnk_03004C30[gUnk_03005220].x & 0x1FF;
        for (i = 0; i <= 159; i++)
            buffer[i] = (*(u16 *)(i * 2 + (u32)gUnk_03005224) + scroll) & 0x1FF;
        break;
    case 1:
        target = ScanlineEffect_GetVerticalScrollRegister(gUnk_03005220);
        scroll = (u16)gUnk_03004C30[gUnk_03005220].y & 0x1FF;
        for (i = 0; i <= 159; i++)
            buffer[i] = (*(u16 *)(i * 2 + (u32)gUnk_03005224) + scroll) & 0x1FF;
        break;
    case 2:
        target = (vu16 *)0x0400004C;
        for (i = 0; i <= 159; i++)
            buffer[i] = *(u16 *)(i * 2 + (u32)gUnk_03005224);
        break;
    case 3:
        *(vu16 *)0x04000052 = 0x810;
        *(vu16 *)0x04000050 = 0xEDE;
        target = (vu16 *)0x04000054;
        for (i = 0; i <= 159; i++)
            buffer[i] = *(u16 *)(i * 2 + (u32)gUnk_03005224) & 31;
        break;
    }
    gUnk_03004278 = buffer;
    gUnk_03003A00 = target;
    gUnk_03004270 = 1;
    gUnk_03004274 = 1;
    gUnk_03005224 = 0;
}
