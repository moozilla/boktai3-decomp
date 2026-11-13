#define ScanlineEffect_StopTransfer sub_082194D4
#define Mosaic_UploadRegisters      sub_08219134
#define TextWindow_LoadFramePalette sub_0821907C
#include "scanline_effect.h"
#include "engine.h"
#include "render.h"
#include "background_controller.h"

/* TextWindow_LoadFramePalette: Selects one of two resources and copies 32 palette colors into RAM; tentative frame palette role */

void TextWindow_LoadFramePalette(u32 mode)
{
    u32 key;
    u8 *res;
    switch (mode) {
        case 0:
        default:
            key = 0x529c;
            break;
        case 1:
            key = 0x539c;
            break;
    }
    res = ScriptProc_Start(0x92b3, key);
    CpuSet(res + 0x1d4, gBgPaletteBank14, 0x04000010);
}

void Mosaic_Initialize(void)
{
    gMosaicHalfword = 0;
    gMosaicParam = 0;
    gSpriteMosaicAttr = 0;
    gMosaicByte = 0;
}

void Mosaic_SetParameters(u16 a, u8 b, u8 c)
{
    gMosaicHalfword = 1;
    gMosaicParam = a;
    gSpriteMosaicAttr = b;
    gMosaicByte = c;
}

void Mosaic_ResetParameters(void)
{
    gMosaicHalfword = 1;
    gMosaicParam = 0;
    gSpriteMosaicAttr = 0;
    gMosaicByte = 0;
}

void Mosaic_UploadRegisters(void)
{
    if (gMosaicHalfword) {
        if (gMosaicByte & 1)
            REG_BG0CNT |= 0x40;
        else
            REG_BG0CNT &= ~0x40;
        if (gMosaicByte & 2)
            REG_BG1CNT |= 0x40;
        else
            REG_BG1CNT &= ~0x40;
        if (gMosaicByte & 4)
            REG_BG2CNT |= 0x40;
        else
            REG_BG2CNT &= ~0x40;
        if (gMosaicByte & 8)
            REG_BG3CNT |= 0x40;
        else
            REG_BG3CNT &= ~0x40;
        if (!(gMosaicByte & 0x10))
            REG_MOSAIC = *(u8 *)&gMosaicParam;
        else
            REG_MOSAIC = gMosaicParam;
        gMosaicHalfword = 0;
    }
}

void ScanlineEffect_Initialize(void)
{
    gScanlineEffectLayer = 2;
    gScanlineEffectMode = 0;
    gScanlineEffectSource = 0;
}

void ScanlineEffect_SetSource(u32 layer, u32 mode, u16 *source)
{
    gScanlineEffectLayer = layer;
    gScanlineEffectMode = mode;
    gScanlineEffectSource = source;
}

u8 *ScanlineEffect_GetBuffer(void)
{
    return (u8 *)0x02036C00;
}

vu16 *ScanlineEffect_GetHorizontalScrollRegister(s32 n)
{
    switch (n) {
        case 0:
            return (vu16 *)0x04000010;
        case 1:
            return (vu16 *)0x04000014;
        case 2:
            return (vu16 *)0x04000018;
        default:
            return (vu16 *)0x0400001C;
    }
}

vu16 *ScanlineEffect_GetVerticalScrollRegister(s32 n)
{
    switch (n) {
        case 0:
            return (vu16 *)0x04000012;
        case 1:
            return (vu16 *)0x04000016;
        case 2:
            return (vu16 *)0x0400001A;
        default:
            return (vu16 *)0x0400001E;
    }
}

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
    switch ((s32)gScanlineEffectMode) {
        case 0:
            target = ScanlineEffect_GetHorizontalScrollRegister((s32)gScanlineEffectLayer);
            scroll = (u16)gBgRecords[(s32)gScanlineEffectLayer].x & 0x1FF;
            for (i = 0; i <= 159; i++)
                buffer[i] = (*(u16 *)(i * 2 + (u32)gScanlineEffectSource) + scroll) & 0x1FF;
            break;
        case 1:
            target = ScanlineEffect_GetVerticalScrollRegister((s32)gScanlineEffectLayer);
            scroll = (u16)gBgRecords[(s32)gScanlineEffectLayer].y & 0x1FF;
            for (i = 0; i <= 159; i++)
                buffer[i] = (*(u16 *)(i * 2 + (u32)gScanlineEffectSource) + scroll) & 0x1FF;
            break;
        case 2:
            target = &REG_MOSAIC;
            for (i = 0; i <= 159; i++)
                buffer[i] = *(u16 *)(i * 2 + (u32)gScanlineEffectSource);
            break;
        case 3:
            REG_BLDALPHA = 0x810;
            REG_BLDCNT = 0xEDE;
            target = &REG_BLDY;
            for (i = 0; i <= 159; i++)
                buffer[i] = *(u16 *)(i * 2 + (u32)gScanlineEffectSource) & 31;
            break;
    }
    gScanlineDmaSource = buffer;
    gScanlineDmaTarget = target;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 1;
    gScanlineEffectSource = 0;
}

void Memory_Clear(void *, s32);

void Mosaic_ResetParameters(void);

void ScanlineEffect_StopTransfer(void)
{
    u16 *buffer;
    vu16 *target;
    buffer = (u16 *)ScanlineEffect_GetBuffer();
    Memory_Clear(buffer, 320);
    target = (vu16 *)0x0400001C;
    switch ((s32)gScanlineEffectMode) {
        case 0:
            target = ScanlineEffect_GetHorizontalScrollRegister(gScanlineEffectLayer);
            break;
        case 1:
            target = ScanlineEffect_GetVerticalScrollRegister(gScanlineEffectLayer);
            break;
        case 2:
            Mosaic_ResetParameters();
            target = &REG_MOSAIC;
            break;
        case 3:
            target = &REG_BLDY;
            break;
    }
    gScanlineDmaSource = buffer;
    gScanlineDmaTarget = target;
    gScanlineDmaCount = 1;
    gScanlineDmaMode = 0;
    gScanlineEffectSource = 0;
}
