#define Bg_GetMapRow            sub_0821607C
#define Bg_FillTilemapRectangle sub_08215EB4
#define Bg_CopyTilemapRectangle sub_08215E24
#define Bg_UploadTileEntries    sub_08215D40
#define Bg_SetCharacterBase     sub_08215AC4
#define Bg_UploadMapsAndTiles   sub_08215964
#define Bg_ResetDisplay         sub_082157EC
#define Bg_Initialize           sub_082156D8
#include "background_layers.h"
#include "ram.h"
#include "render.h"
#include "gba/io_reg.h"

u8 *Video_GetBackgroundMap(u32 n)
{
    return gBgMapWorkBuffer + (n << 11);
}

void Memory_Clear(u32, u32);

void Bg_ClearAllTilemaps(void)
{
    Memory_Clear(0x02037000, 0x2000);
}

void Text_ClearGlyphCache(void);

u8 *Video_GetBackgroundMap(u32);

void Bg_ClearTilemap(u32 n)
{
    if (n == 0)
        Text_ClearGlyphCache();
    Memory_Clear(Video_GetBackgroundMap(n), 0x800);
}

void Bg_ClearAllTilemaps(void);

void Bg_Initialize(u32 mode)
{
    s32 i;

    Bg_ClearAllTilemaps();
    for (i = 0; i < 4; i++) {
        gBgTileUploadSrc[i] = 0;
        gBgTileUploadParamA[i] = 0;
        gBgTileUploadParamB[i] = 0;
    }
    gDispCntShadow = 0;
    gBgAffineScaleX = 0x100;
    gBgAffineScaleY = 0x100;
    gBgAffineAngle = 0;
    gBgAffineMatrix[0] = 0x100;
    gBgAffineMatrix[1] = 0;
    gBgAffineMatrix[2] = 0;
    gBgAffineMatrix[3] = 0x100;
    gBgAffineRefX = 0;
    gBgAffineRefY = 0;
    gBgAffineMode = mode;
    switch (mode) {
        case 0:
            REG_BG0CNT = 0x3F08;
            REG_BG1CNT = 0x3E01;
            REG_BG2CNT = 0x3D03;
            REG_BG3CNT = 0x3C01;
            REG_DISPCNT = 0x7160;
            break;
        case 1:
            REG_BG0CNT = 0x3F08;
            REG_BG1CNT = 0x3E42;
            REG_BG2CNT = 0x9D81;
            REG_DISPCNT = 0x7161;
            break;
    }
    {
        s32 mask = ~0xE00;
        gDispCntShadow &= mask;
    }
}

extern const u32 gUnk_086140CC[];

void Bg_ClearTilemap(u32);

void Bg_ResetDisplay(void)
{
    u32 zero;
    s32 i;
    gDispCntShadow = 0;
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    REG_BG0CNT &= 0xffbf;
    REG_BG1CNT &= 0xffbf;
    REG_BG2CNT &= 0xffbf;
    REG_BG3CNT &= 0xffbf;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    REG_BLDY = 0;
    i = 0;
    zero = 0;
    for (; i <= 3; i++) {
        struct VideoDmaRegisters *dma = (struct VideoDmaRegisters *)0x040000d4;
        gBgTileUploadSrc[i] = zero;
        gBgTileUploadParamA[i] = zero;
        gBgTileUploadParamB[i] = zero;
        dma->source = (u32)gUnk_086140CC;
        dma->destination = 0x06000000 + i * 0x4000;
        dma->control = 0x84000008;
        dma->control;
    }
}

void Bg_UpdateScrollShadow(void)
{
    u16 *dst = gBgScrollShadow;
    struct BackgroundLayer *src = gBgRecords;
    u32 mask;
    dst[0] = ((u16)src[0].x) & 0xff;
    mask = 0xff;
    dst[1] = mask & ((u16)src[0].y);
    dst[2] = mask & ((u16)src[1].x);
    dst[3] = mask & ((u16)src[1].y);
    if (!gBgAffineMode) {
        dst[4] = mask & ((u16)src[2].x);
        dst[5] = mask & ((u16)src[2].y);
        {
            u16 *scroll = (u16 *)&src[3].x;
            dst[6] = mask & (u16)*scroll;
        }
        {
            u16 *scroll = (u16 *)&src[3].y;
            dst[7] = mask & (u16)*scroll;
        }
    } else {
        u16 *a = gBgAffineMatrixBuffer, *b = gBgAffineMatrix;
        u32 *c;
        a[0] = b[0];
        a[1] = b[1];
        a[2] = b[2];
        a[3] = b[3];
        c = gBgAffineRefBuffer;
        c[0] = gBgAffineRefX;
        c[1] = gBgAffineRefY;
    }
}

u8 *Video_GetBackgroundMap(u32);

void BgGfx_LoadTiles(u8 *source, u8 *destination, u32 bytes);

#define CopyMap(layer, dest)                                                                       \
    {                                                                                              \
        struct VideoDmaRegisters *d = (struct VideoDmaRegisters *)0x040000D4;                      \
        d->source = (u32)Video_GetBackgroundMap(layer);                                            \
        d->destination = (dest);                                                                   \
        d->control = 0x84000200;                                                                   \
        d->control;                                                                                \
    }

void Bg_UploadMapsAndTiles(void)
{
    s32 i;

    if (gDispCntShadow & 0x100)
        CopyMap(0, 0x0600F800);
    if (gDispCntShadow & 0x200)
        CopyMap(1, 0x0600F000);
    if (gDispCntShadow & 0x400)
        CopyMap(2, 0x0600E800);
    if (gDispCntShadow & 0x800)
        CopyMap(3, 0x0600E000);
    for (i = 0; i <= 3; i++) {
        if (gBgTileUploadSrc[i]) {
            BgGfx_LoadTiles(gBgTileUploadSrc[i], (u8 *)0x06000000 + gBgTileUploadParamB[i],
                            gBgTileUploadParamA[i] << 5);
            gBgTileUploadSrc[i] = 0;
        }
    }
}

void Bg_SetTileUpload(u32 i, u32 a, u32 b, u32 c)
{
    gBgTileUploadSrc[i] = a;
    gBgTileUploadParamA[i] = c;
    gBgTileUploadParamB[i] = b;
}

void Bg_SetPriority(s32 n, u32 v)
{
    vu16 *r;
    u16 t;
    switch (n) {
        case 0:
            r = (vu16 *)0x04000008;
            break;
        case 1:
            r = (vu16 *)0x0400000A;
            break;
        case 2:
            r = (vu16 *)0x0400000C;
            break;
        case 3:
            r = (vu16 *)0x0400000E;
            break;
        default:
            return;
    }
    t = (0xFFFC & *r) | (v);
    *r = t;
}

// Sets the character-base block (BGxCNT bits 2-3) of background `bg`.

void Bg_SetCharacterBase(s32 bg, u32 charBase)
{
    switch (bg) {
        case 0: {
            u16 v = (REG_BG0CNT & 0xFFF3) | (charBase << 2);
            REG_BG0CNT = v;
        } break;
        case 1: {
            u16 v = (REG_BG1CNT & 0xFFF3) | (charBase << 2);
            REG_BG1CNT = v;
        } break;
        case 2: {
            u16 v = (REG_BG2CNT & 0xFFF3) | (charBase << 2);
            REG_BG2CNT = v;
        } break;
        case 3: {
            u16 v = (REG_BG3CNT & 0xFFF3) | (charBase << 2);
            REG_BG3CNT = v;
        } break;
    }
}

extern s16 gUnk_03004C10;

extern s16 gUnk_03004BE8;

extern const s16 gUnk_086149C4[];

s32 __divsi3(s32, s32);

/* The 8.8 reciprocal is narrowed before multiplication by the wave sample. */

static inline s16 FixedDivide(s32 numerator, s16 scale)
{
    union {
        s32 dividend;
        s32 reciprocal;
    } value;
    value.dividend = numerator << 8;
    value.reciprocal = __divsi3(value.dividend, scale);
    return value.reciprocal;
}

/* Division by 256 rounds negative products toward zero. */

static inline s16 FixedMultiply(s32 a, s32 b)
{
    s32 product = a * b;
    product /= 256;
    return product;
}

void Bg_SetAffineTransform(s32 x, s32 y, s32 angle, u16 scaleX, s32 scaleY)
{
    gUnk_03004C10 = x;
    gUnk_03004BE8 = y;
    gBgAffineAngle = angle;
    gBgAffineScaleX = scaleX;
    gBgAffineScaleY = scaleY;
    /* The table uses 12 fractional bits; the affine matrix uses eight. */
    gBgAffineMatrix[0] = FixedMultiply((gUnk_086149C4[(gBgAffineAngle + 64) & 255] >> 4),
                                       FixedDivide(256, gBgAffineScaleX));
    gBgAffineMatrix[1] = FixedMultiply((gUnk_086149C4[gBgAffineAngle & 255] >> 4),
                                       FixedDivide(256, gBgAffineScaleX));
    gBgAffineMatrix[2] = FixedMultiply((-gUnk_086149C4[gBgAffineAngle & 255] >> 4),
                                       FixedDivide(256, gBgAffineScaleY));
    gBgAffineMatrix[3] = FixedMultiply((gUnk_086149C4[(gBgAffineAngle + 64) & 255] >> 4),
                                       FixedDivide(256, gBgAffineScaleY));
    gBgAffineRefX =
        ((120 - gUnk_03004C10) << 8) - 120 * gBgAffineMatrix[0] - 80 * gBgAffineMatrix[1];
    gBgAffineRefY =
        ((120 - gUnk_03004BE8) << 8) - 120 * gBgAffineMatrix[2] - 80 * gBgAffineMatrix[3];
}

struct BgTileEntry *BgGfx_FindEntryById(struct BgTileResource *h, u16 id)
{
    struct BgTileEntry *e = h->entries;
    s32 i = 0;
    for (; i < h->count; e++, i++) {
        if (e->id == id)
            return e;
    }
    return 0;
}

struct BgTileEntry *BgGfx_FindEntryById(struct BgTileResource *h, u16 id);

void CpuSet(const void *src, void *dst, u32 ctl);

s32 Bg_CopyTiles(struct BgTileResource *res, u32 id, u8 *dst, s32 start, u32 count)
{
    struct BgTileResource hdr;
    struct BgTileEntry *ent;
    u16 *p;
    s32 off;
    u32 i;
    hdr = *res;
    hdr.entries += (u32)res;
    hdr.tileIndices += (u32)res;
    hdr.tiles += (u32)res;
    ent = BgGfx_FindEntryById(&hdr, (u16)id);
    if (ent == 0)
        return -1;
    off = 0;
    p = (u16 *)(ent->offset + hdr.tileIndices);
    p += start;
    for (i = 0; i < count; i++) {
        CpuSet((u8 *)hdr.tiles + (*p << 5), dst + off, 0x04000008);
        p++;
        off += 0x20;
    }
    return 0;
}

s32 Bg_UploadTileEntries(s32 layer, struct BgTileResource *res, s32 count, u16 *ids, s32 start)
{
    struct BgTileResource hdr;
    struct BgTileEntry *ent;
    u16 *p;
    s32 i, j;
    u32 dst, off;

    hdr = *res;
    hdr.entries += (u32)res;
    hdr.tileIndices += (u32)res;
    hdr.tiles += (u32)res;
    off = start << 5;
    if (layer == 0)
        off = 0x8000;
    dst = off;
    for (i = 0; i < count; i++) {
        ent = BgGfx_FindEntryById(&hdr, ids[i]);
        p = (u16 *)(ent->offset + hdr.tileIndices);
        for (j = 0; j < ent->count; j++) {
            u32 s = hdr.tiles + (*p << 5);
            u32 t = dst + 0x06000000;
            {
                struct VideoDmaRegisters *d = (struct VideoDmaRegisters *)0x040000D4;
                d->source = s;
                d->destination = t;
                d->control = 0x84000008;
                d->control;
            }
            p++;
            dst += 0x20;
        }
    }
    return 0;
}

void Bg_SetTilemapEntry(u32 n, s32 x, s32 y, u32 a, u32 b, u32 c)
{
    struct BackgroundLayer *bg = &gBgRecords[n];
    bg->map[(bg->width << 1) * y + x] = a | (b << 10) | (c << 12);
}

void Bg_CopyTilemapRectangle(s32 layer, s32 x, s32 y, s32 w, s32 h, s32 *src, s32 palette,
                             s32 priority)
{
    //BODY
    struct BackgroundLayer *l = &gBgRecords[layer];
    s32 stride;
    u16 *map;
    s32 xEnd, yEnd, x0;
    map = l->map;
    stride = l->width * 2;
    xEnd = x + w;
    yEnd = h + y;
    x0 = x;
    for (; y < yEnd; y++) {
        for (x = x0; x < xEnd; x++) {
            map[stride * y + x] = *src++ | (palette << 10) | (priority << 12);
        }
    }
}

void Bg_FillTilemapRectangle(s32 layer, s32 x, s32 y, s32 w, s32 h, s32 tile, s32 palette,
                             s32 priority)
{
    //BODY
    struct BackgroundLayer *l = &gBgRecords[layer];
    u16 *map = l->map;
    s32 stride = l->width * 2;
    s32 xEnd = x + w;
    s32 yEnd = h + y;
    s32 x0 = x;
    for (; y < yEnd; y++) {
        for (x = x0; x < xEnd; x++) {
            map[stride * y + x] = tile | (palette << 10) | (priority << 12);
        }
    }
}

void Bg_CopyTilemapRectangle(s32, s32, s32, s32, s32, s32 *, s32, s32);

#define DIGIT(i, t)                                                                                \
    {                                                                                              \
        d = base;                                                                                  \
        while (value > (t) - 1) {                                                                  \
            d++;                                                                                   \
            value -= (t);                                                                          \
        }                                                                                          \
        digits[n] = d;                                                                             \
        n++;                                                                                       \
    }

void Text_DrawNumber(s32 layer, s32 x, s32 y, u32 value, s32 base, s32 count, s32 arg7)
{
    s32 digits[8];
    s32 d;
    s32 n;
    s32 rest;
    if (count > 8)
        count = 8;
    n = 0;
    DIGIT(0, 10000000)
    DIGIT(1, 1000000)
    DIGIT(2, 100000)
    DIGIT(3, 10000)
    DIGIT(4, 1000)
    DIGIT(5, 100)
    DIGIT(6, 10)
    DIGIT(7, 1)
    rest = n - count;
    Bg_CopyTilemapRectangle(layer, x, y, count, 1, digits + rest, 0, arg7);
}

/* BgGfx_GetMapRow: tentatively returns an indexed row of 16-bit entries from resource map data */

void *memcpy(void *, const void *, u32);

u8 *Bg_GetMapRow(u8 *base, s32 i)
{
    struct BgLayerResource h;
    memcpy(&h, base, 64);
    h.extraData += (u32)base;
    h.mapData += (u32)base;
    h.mapRows += (u32)base;
    h.tiles += (u32)base;
    return (u8 *)h.mapRows + h.rowWidth * i * 2;
}

void Bg_SetTileUpload(u32, u32, u32, u32);

void Bg_SetPriority(s32, u32);

void Bg_SetupLayer(s32 layer, u16 mode, const struct BgLayerResource *resource, s32 unused, s32 x,
                   s32 y, s32 priority, u16 *map)
{
    struct BgLayerResource hdr;
    u32 tileOffset;
    s32 px, py;
    struct BackgroundLayer *st = &gBgRecords[layer];
    st->flags = mode;
    st->layer = layer;
    st->x = 0;
    st->y = 0;
    st->scaleX = 0x1000;
    st->scaleY = 0x1000;
    px = (s16)x >> 4;
    py = (s16)y >> 4;
    memcpy(&hdr, resource, sizeof(hdr));
    hdr.extraData += (u32)resource;
    hdr.mapData += (u32)resource;
    hdr.mapRows += (u32)resource;
    hdr.tiles += (u32)resource;
    tileOffset = 0;
    switch (layer) {
        case 0:
            tileOffset = 0x8000;
            Bg_EnableDisplay(DISPCNT_BG0_ON);
            break;
        case 1:
            tileOffset = hdr.tileIndex * 32;
            Bg_EnableDisplay(DISPCNT_BG1_ON);
            break;
        case 2:
            tileOffset = hdr.tileIndex * 32;
            Bg_EnableDisplay(DISPCNT_BG2_ON);
            break;
        case 3:
            tileOffset = hdr.tileIndex * 32;
            Bg_EnableDisplay(DISPCNT_BG3_ON);
            break;
    }
    Bg_SetTileUpload(layer, hdr.tiles, tileOffset, hdr.tileCount);
    Bg_SetPriority(layer, priority);
    st->originX = px;
    st->originY = py;
    st->width = hdr.width;
    st->height = hdr.height;
    st->extraData = (u16 *)hdr.extraData;
    st->mapData = (u16 *)hdr.mapData;
    st->map = map;
    if (layer == 0)
        Text_ClearGlyphCache();
}

void Bg_SetCharacterBase(s32 bg, u32 mode);

/* Sets up the background layers of a map resource for one of eight layer
   modes: picks the BGs (Bg_SetCharacterBase) and DISPCNT bits, loads the tiles
   (Bg_SetTileUpload) and fills each layer's gBgRecords entry from the map
   entries, picked through maps[] for the first count layers. */

void Bg_SetupLayers(s32 mode, u16 resourceId, struct BgMapResource *res, u16 x, u16 y, s32 count,
                    s32 *maps)
{
    s32 bgs[4];
    struct BgMapResource hdr;
    struct BgMapEntry entry;
    s32 n, i;
    s32 offset;
    hdr = *res;
    hdr.maps += (u32)res;
    hdr.tiles += (u32)res;
    hdr.extra += (u32)res;
    switch (mode) {
        case 0:
            bgs[0] = 0;
            n = 1;
            offset = 0x8000;
            Bg_EnableDisplay(0x100);
            Bg_SetCharacterBase(0, 2);
            break;
        case 1:
            Bg_SetCharacterBase(bgs[0] = 2, 0);
            if (count == 2) {
                Bg_EnableDisplay(0x800);
                Bg_SetCharacterBase(bgs[1] = 3, 0);
                n = 2;
            } else
                n = 1;
            offset = hdr.tileCount << 5;
            Bg_EnableDisplay(0x400);
            break;
        case 2:
            Bg_SetCharacterBase(bgs[0] = 3, 0);
            n = 1;
            offset = hdr.tileCount << 5;
            Bg_EnableDisplay(0x800);
            break;
        case 3:
            Bg_SetCharacterBase(bgs[0] = 1, 0);
            n = 1;
            offset = hdr.tileCount << 5;
            Bg_EnableDisplay(0x200);
            break;
        case 4:
            Bg_SetCharacterBase(bgs[0] = 1, 0);
            if (count == 2) {
                Bg_EnableDisplay(0x800);
                Bg_SetCharacterBase(bgs[1] = 3, 0);
                n = 2;
            } else
                n = 1;
            offset = hdr.tileCount << 5;
            Bg_EnableDisplay(0x200);
            break;
        case 5:
            Bg_SetCharacterBase(bgs[0] = 2, 0);
            if (count == 2) {
                Bg_EnableDisplay(0x200);
                Bg_SetCharacterBase(bgs[1] = 1, 0);
                n = 2;
            } else
                n = 1;
            offset = hdr.tileCount << 5;
            Bg_EnableDisplay(0x400);
            break;
        case 6:
            Bg_SetCharacterBase(bgs[0] = 1, 2);
            n = 1;
            offset = 0x8000;
            Bg_EnableDisplay(0x200);
            break;
        case 7:
            n = 3;
            bgs[0] = 2;
            bgs[1] = 3;
            bgs[2] = 1;
            Bg_SetCharacterBase(2, 0);
            Bg_SetCharacterBase(3, 0);
            Bg_SetCharacterBase(1, 0);
            {
                Bg_EnableDisplay(0x200);
                Bg_EnableDisplay(0x400);
                Bg_EnableDisplay(0x800);
            }
            offset = hdr.tileCount << 5;
            break;
        default:
            Bg_SetCharacterBase(bgs[0] = 2, 0);
            n = 0;
            offset = 0;
            break;
    }
    if (hdr.count && hdr.tiles != (u32)res)
        Bg_SetTileUpload(bgs[0], hdr.tiles, offset, hdr.count);
    for (i = 0; i < n; i++) {
        s32 bg = bgs[i];
        struct BackgroundLayer *layer = &gBgRecords[bg];
        struct BgMapEntry *entries = (struct BgMapEntry *)hdr.maps;
        if (bg == 0)
            Text_ClearGlyphCache();
        if (i < count && maps)
            entry = entries[maps[i]];
        else
            entry = entries[i];
        entry.data += (u32)res;
        layer->flags = resourceId;
        layer->layer = bg;
        layer->x = 0;
        layer->y = 0;
        layer->scaleX = 0x1000;
        layer->scaleY = 0x1000;
        {
            s16 lx = (s16)x >> 4, ly = (s16)y >> 4;
            layer->originX = lx;
            layer->originY = ly;
        }
        layer->extraData = (u16 *)hdr.extra;
        layer->mapData = (u16 *)entry.data;
        {
            u16 w, h;
            layer->width = w = entry.width;
            layer->height = h = entry.height;
            layer->halfWidth = w;
            layer->halfHeight = h;
        }
        layer->map = Video_GetBackgroundMap(bg);
    }
}

void Bg_ResetScale(u32 n)
{
    struct BackgroundLayer *bg = &gBgRecords[n];
    bg->scaleX = 0x1000;
    bg->scaleY = 0x1000;
}

/* Points background layer bg at map entry index of a map resource (data,
   size, scale 1.0); see Bg_SetupLayers. */

void Bg_SetLayerMap(s32 bg, struct BgMapResource *res, s32 index)
{
    struct BgMapResource hdr;
    struct BgMapEntry entry;
    struct BackgroundLayer *layer;
    if (bg == 0)
        Text_ClearGlyphCache();
    hdr = *res;
    hdr.maps += (u32)res;
    hdr.tiles += (u32)res;
    hdr.extra += (u32)res;
    layer = &gBgRecords[bg];
    entry = ((struct BgMapEntry *)hdr.maps)[index];
    entry.data += (u32)res;
    layer->scaleX = 0x1000;
    layer->scaleY = 0x1000;
    layer->mapData = (u16 *)entry.data;
    {
        u16 w, h;
        layer->width = w = entry.width;
        layer->height = h = entry.height;
        layer->halfWidth = w;
        layer->halfHeight = h;
    }
}
