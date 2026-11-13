#ifndef GUARD_TEXT_GRAPHICS_H
#define GUARD_TEXT_GRAPHICS_H
#include "text.h"
#include "render.h"
#include "interrupts_video.h"
u16 *Video_GetBackgroundMap(u32);
s32 Text_ReleaseCachedGlyph(u32);
#endif
