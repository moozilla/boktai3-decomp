#define Text_GetNarrowGlyphCount sub_08218C40
#define Text_ResetFontResource   sub_08218BC8
#include "text_graphics.h"
#include "render.h"
#include "text.h"

void Video_OnFrameNoOp(void)
{
}

void Text_FlushTileUploads(void)
{
    u32 i;
    u32 src = 0x02034C00;
    for (i = 0; i < gTextTileUploadCount; i++) {
        u32 dst = (gTextTileUploadIndices[i] << 5) + 0x0600A000;
        struct VideoDmaRegisters *d = (struct VideoDmaRegisters *)0x040000D4;
        d->source = src;
        d->destination = dst;
        d->control = 0x84000008;
        d->control;
        src += 0x20;
    }
    gTextTileUploadCount = 0;
}

void Memory_Copy(u32, u32, u32);

void Text_QueueTileUpload(s32 index, u32 *tile)
{
    if (gTextTileUploadCount <= 0x100) {
        gTextTileUploadIndices[gTextTileUploadCount] = index;
        Memory_Copy(gTextTileUploadCount * 32 + 0x02034C00, (u32)tile, 0x20);
        gTextTileUploadCount++;
    }
}

/* Text_ResetFontResource: Clears the global font resource pointer without freeing it */

void Text_ResetFontResource(void)
{
    gFontResource = 0;
}

struct FontResource *Text_GetFontResource(void)
{
    return gFontResource;
}

struct FontResource *Heap_Alloc(u32);

void Memory_Clear(void *, u32);

struct FontResource *ScriptProc_Start(u32, u32);

s32 Text_LoadFontResource(void)
{
    if (gFontResource != 0)
        return 0;
    gFontResource = Heap_Alloc(12);
    if (gFontResource != 0) {
        struct FontResource *p;
        Memory_Clear(gFontResource, 12);
        p = ScriptProc_Start(0xA635, 0x3F51);
        if (p != 0) {
            *gFontResource = *p;
            gFontResource->narrowGlyphData += (u32)p;
            gFontResource->wideGlyphData += (u32)p;
            return 0;
        }
    }
    return -1;
}

/* Text_GetNarrowGlyphCount: Returns the first font header count provisionally interpreted as the narrow glyph count */

u16 Text_GetNarrowGlyphCount(void)
{
    if (gFontResource == NULL)
        return 0;
    return gFontResource->narrowGlyphCount;
}

u16 Text_GetWideGlyphCount(void)
{
    if (gFontResource == 0)
        return 0;
    return gFontResource->wideGlyphCount;
}

u8 *Text_GetNarrowGlyphData(void)
{
    if (gFontResource == 0)
        return 0;
    return (u8 *)gFontResource->narrowGlyphData;
}

u8 *Text_GetWideGlyphData(void)
{
    if (gFontResource == 0)
        return 0;
    return (u8 *)gFontResource->wideGlyphData;
}

void Memory_Clear(void *, u32);

void Text_ClearGlyphCache(void)
{
    Memory_Clear((void *)0x02034400, 0x800);
}

u32 *Text_GetGlyphCache(void)
{
    return (u32 *)0x02034400;
}

u32 *Text_GetGlyphCache(void);

s32 Text_FindCachedGlyph(u32 glyph, u32 part, u32 color, u32 wide)
{
    u32 key = (glyph | (part << 16) | (color << 18) | (wide << 20)) & 0x3FFFFF;
    u32 *p = Text_GetGlyphCache();
    s32 i = 0;
    do {
        u32 v = *p;
        if ((v & 0x3FFFFF) == key && (v & 0x3FC00000) != 0)
            return i;
        p++;
        i++;
    } while (i <= 0xFF);
    return -1;
}

s32 Text_FindCachedGlyph(u32 glyph, u32 part, u32 color, u32 wide);

void Text_QueueTileUpload(s32 index, u32 *tile);

s32 Text_DrawGlyph(u32 *data, u32 glyph, u32 part, u32 color, u32 wide)
{
    u32 *cache = Text_GetGlyphCache();
    s32 index = Text_FindCachedGlyph(glyph, part, color, wide);
    u32 tile[8];
    u32 hi, lo, fill;
    s32 i;
    u32 *dst;
    u32 *src;

    if (index >= 0) {
        u32 *e = (u32 *)((index << 2) + (u32)cache);
        u32 v = *e;
        u32 cnt = v & 0x3FC00000;
        *e = (v & 0xC03FFFFF) | (cnt + 0x400000);
        return index;
    }
    for (i = 0; i <= 0xFF; cache++, i++) {
        if (!(*cache & 0x3FC00000))
            break;
    }
    if (i == 0x100)
        return -1;
    index = i;
    *cache = glyph | (part << 16) | (color << 18) | (wide << 20) | 0x400000;
    fill = color * 0x01010101;
    hi = fill << 6;
    lo = fill << 2;
    src = data;
    {
        u32 mhi = 0xF0F0F0F0;
        u32 mlo = 0x0F0F0F0F;
        dst = tile;
        for (i = 7; i >= 0; i--) {
            u32 w = *src++;
            *dst++ = (((w & mhi) | hi) & mhi) | (((w & mlo) | lo) & mlo);
        }
    }
    Text_QueueTileUpload(index, tile);
    return index;
}

s32 Text_ReleaseCachedGlyph(u32 idx)
{
    u32 *tbl = Text_GetGlyphCache();
    idx -= 0x100;
    if (idx <= 0xFF) {
        u32 *e = (u32 *)((idx << 2) + (u32)tbl);
        u32 v = *e;
        s32 cnt = (v & 0x3FC00000) >> 22;
        if (cnt > 0) {
            cnt--;
            if (cnt <= 0)
                *e = 0;
            else
                *e = (v & 0xC03FFFFF) | (cnt << 22);
            return 1;
        }
    }
    return 0;
}

u16 *Video_GetBackgroundMap(u32);

s32 Text_ReleaseCachedGlyph(u32);

void TextWindow_BlankRect(u32 x, u32 y, u32 w, u32 h)
{
    u16 *map;
    u32 xEnd, yEnd, x0, left;

    map = (u16 *)Video_GetBackgroundMap(0);
    xEnd = (x0 = x) + w;
    yEnd = h + y;
    for (; y < yEnd; y++) {
        left = x0;
        for (x = left; x < xEnd; x++) {
            u16 *p = (u16 *)(((y * 32 + x) * 2) + (u32)map);
            if (Text_ReleaseCachedGlyph(*p & 0x3FF) != 0)
                *p = 0xF001;
        }
    }
}

void BackgroundMap_ClearTextRect(u32 x, u32 y, u32 width, u32 height)
{
    u16 *map = Video_GetBackgroundMap(0);
    u32 start = x;
    u32 right = x + width;
    u32 bottom = y + height;
    for (; y < bottom; y++) {
        for (x = start; x < right; x++)
            map[y * 32 + x] = 0xF001;
    }
}

struct FontResource *Text_GetFontResource(void);

s32 Text_LoadFontResource(void);

u8 *Text_GetNarrowGlyphData(void);

s32 Text_DrawGlyph(u32 *data, u32 glyph, u32 part, u32 arg, u32 wide);

void TextWindow_DrawNarrowGlyph(u16 glyph, s32 x, s32 y, u32 arg)
{
    u8 *data;
    u16 *map;
    s32 i;

    if (Text_GetFontResource() == 0 && Text_LoadFontResource() < 0)
        return;
    data = Text_GetNarrowGlyphData() + glyph * 64;
    map = Video_GetBackgroundMap(0);
    for (i = 0; i < 2; i++) {
        s32 tile = Text_DrawGlyph((u32 *)data, glyph, i, arg, 0);
        if (tile < 0)
            return;
        tile += 0x100;
        data += 0x20;
        map[y * 32 + x] = tile | ~0xFFF;
        y = (y + 1) & 0x1F;
    }
}

u8 *Text_GetWideGlyphData(void);

void TextWindow_DrawWideGlyph(u16 glyph, s32 x, s32 y, u32 arg)
{
    u8 *data;
    u16 *map;
    s32 i, j;

    if (Text_GetFontResource() == 0 && Text_LoadFontResource() < 0)
        return;
    data = Text_GetWideGlyphData() + glyph * 128;
    map = Video_GetBackgroundMap(0);
    for (i = 0; i < 2; i++) {
        u32 rowOff = ((y + i) & 0x1F) << 5;
        for (j = 0; j < 2; j++) {
            s32 tile = Text_DrawGlyph((u32 *)data, glyph, i * 2 + j, arg, 1);
            if (tile < 0)
                return;
            tile += 0x100;
            data += 0x20;
            *(u16 *)(((((x + j) & 0x1F) + rowOff) * 2) + (u32)map) = tile | ~0xFFF;
        }
    }
}

void TextWindow_InitializeGraphics(void)
{
}

void TextWindow_FillTileGraphics(void)
{
    u32 v = 0x44444444;
    CpuFastSet(&v, (void *)0x0600A000, 0x01000800);
}
