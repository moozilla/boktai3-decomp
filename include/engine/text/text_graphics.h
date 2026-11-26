#ifndef GUARD_TEXT_GRAPHICS_H
#define GUARD_TEXT_GRAPHICS_H
#include "engine/text/text.h"
#include "engine/shared/render.h"
#include "engine/interrupts_and_video/interrupts_video.h"
u16 *Video_GetBackgroundMap(u32);
s32 Text_ReleaseCachedGlyph(u32);
#endif
