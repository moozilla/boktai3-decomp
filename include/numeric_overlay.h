#ifndef NUMERIC_OVERLAY_H
#define NUMERIC_OVERLAY_H
#include "global.h"
#include "engine.h"
#include "status_screen.h"

// Tentative: a lightweight sibling of the status screen: it loads the same background map and
// palette, draws the same counter display (sub_0800F4EC mirrors StatusScreen_DrawClock) but has no fades
// or scanline transition. Its task object is 0x2C bytes.
struct NumericOverlay {
    u8 task[24];
    u16 id;
    u8 flags, bg;
    u16 mapId, paletteId;
    u16 *buffer; // working palette (gBgPaletteBank13)
    u16 *source; // palette resource
    u16 dirty;   // bit mask of displays to redraw (set to 15 by NumericOverlay_RefreshDisplay)
    u16 pad2A;
};

/* 0x02000048 */
extern struct NumericOverlay *gNumericOverlay; // the live overlay, or NULL

u16 *NumericOverlay_GetTileMapPtr(u32 bg, u32 x, s32 y); // same as StatusScreen_GetTilePtr
void sub_0800F4EC(struct NumericOverlay *);
void NumericOverlay_RefreshDisplay(struct NumericOverlay *);
s32 sub_0800F6F8(struct NumericOverlay *, u16 id);

#endif
