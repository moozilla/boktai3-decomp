#ifndef GUARD_SCANLINE_EFFECT_H
#define GUARD_SCANLINE_EFFECT_H
#include "render.h"
#include "gba/gba.h"
void ScanlineEffect_Initialize(void);
void ScanlineEffect_SetSource(u32 layer, u32 mode, u16 *source);
u8 *ScanlineEffect_GetBuffer(void);
vu16 *ScanlineEffect_GetHorizontalScrollRegister(s32 layer);
vu16 *ScanlineEffect_GetVerticalScrollRegister(s32 layer);
void ScanlineEffect_StageTransfer(void);
#endif
