#ifndef GUARD_TEXT_GRAPHICS_H
#define GUARD_TEXT_GRAPHICS_H
#include "engine/video/text.h"
#include "engine/video/render.h"
#include "engine/video/interrupts_video.h"
u16 *Video_GetBackgroundMap(u32);
s32 Text_ReleaseCachedGlyph(u32);
#endif
