#include "global.h"
#include "engine.h"
#include "render.h"
struct PaletteBlendEntry {
    u16 *source, *target, *base;
    u8 count, fieldD, frames, fieldF, speedS, speedD, w, W, field14;
    u8 pad15[3];
};
struct PaletteBlender {
    u8 pad0[0x18];
    u32 entryCount;
    u16 colors[256];
    struct PaletteBlendEntry entries[16];
};
struct PaletteBlender *PaletteCycle_Create(void);
/* Registry routine 3A89: adds a palette blend entry to the palette blend task
   (0x02000094, created on demand; up to 16): -s and -d palette, color and
   bank (1 = the task's own colors, else the palette buffer), -b palette and
   color in the palette buffer, -n (1), -f, -S (10), -D (10), -w, -W.
   Returns -1 when full (tentative). */
s32 PaletteBlend_AddFromScript(void)
{
    struct PaletteBlender *p = PaletteCycle_Create();
    struct PaletteBlendEntry *e;
    s32 sp, sc, sb, dp, dc, db, bp, bc;
    if (!p)
        return -1;
    if (p->entryCount > 15)
        return -1;
    e = &p->entries[p->entryCount];
    if (Script_SeekToKeyword('s')) {
        sp = Script_GetValue();
        sc = Script_GetValue();
        sb = Script_GetValue();
    } else {
        sp = 0;
        sc = 0;
        sb = 0;
    }
    if (Script_SeekToKeyword('d')) {
        dp = Script_GetValue();
        dc = Script_GetValue();
        db = Script_GetValue();
    } else {
        dp = 0;
        dc = 0;
        db = 0;
    }
    if (Script_SeekToKeyword('b')) {
        bp = Script_GetValue();
        bc = Script_GetValue();
    } else {
        bp = 0;
        bc = 0;
    }
    if (sb == 1)
        e->source = &p->colors[sp * 16 + sc];
    else
        e->source = &((u16(*)[16])gBgPaletteBuffer)[sp][sc];
    if (db == 1)
        e->target = &p->colors[dp * 16 + dc];
    else
        e->target = &((u16(*)[16])gBgPaletteBuffer)[dp][dc];
    e->base = &((u16(*)[16])gBgPaletteBuffer)[bp][bc];
    e->count = Script_GetKeywordValue('n', 1);
    e->fieldD = 0;
    e->field14 = 0;
    e->frames = Script_GetKeywordValue('f', 0);
    e->fieldF = 0;
    e->speedS = Script_GetKeywordValue('S', 10);
    e->speedD = Script_GetKeywordValue('D', 10);
    e->w = Script_GetKeywordValue('w', 0);
    e->W = Script_GetKeywordValue('W', 0);
    p->entryCount++;
}
