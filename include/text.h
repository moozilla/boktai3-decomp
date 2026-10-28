#ifndef GUARD_TEXT_H
#define GUARD_TEXT_H

#include "global.h"

/* Font counts and relocated glyph-data addresses; count interpretations tentative. */
struct FontResource {
    u16 narrowGlyphCount, wideGlyphCount;
    u32 narrowGlyphData, wideGlyphData;
};

/* 0x03001664 */
extern struct FontResource *gFontResource;

#endif
