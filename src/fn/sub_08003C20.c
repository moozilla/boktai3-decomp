#include "global.h"
#include "terrain_overlay.h"
/* Tentative: draws terrain-edge overlays around tracked actors. */
extern struct TerrainOverlayMap *gUnk_030052F4;
u8 *sub_0821D64C(s32, s32);
void sub_08214374(struct TerrainOverlayDrawable *, s32);
void sub_08003B44(struct TerrainOverlay *, u16);
void sub_08003B84(struct TerrainOverlay *, u16);
static inline struct TerrainOverlayCell *baseCell(s32 index)
{
    struct TerrainOverlayCell *cell;
    if (!gUnk_030052F4->cells)
        return 0;
    cell = (struct TerrainOverlayCell *)(gUnk_030052F4->cells + 12 + index * 4);
    return cell;
}
static inline u16 rowOffset(s32 row)
{
    return gUnk_030052F4->rows[row];
}
s32 TerrainOverlay_Update(struct TerrainOverlay *p)
{
    s32 heights[4], tiles[4], xs[3], zs[3], indices[3];
    s32 i, j, used;
    struct TerrainOverlayDrawable *out;
    struct TerrainOverlayEntry *entry;
    if (!p->initialized) {
        p->initialized = 1;
        for (i = 0; i < p->drawableCount; i++) {
            struct TerrainOverlayDrawable *base = p->drawables;
            struct TerrainOverlayDrawable *d = (struct TerrainOverlayDrawable *)(sizeof(*base) * i + (u32)base);
            if (!d->active)
                sub_08214374(d, 0);
        }
    }
    for (i = 0; (u32)i < p->count; i++) {
        switch (p->commands[i][0]) {
            case 0:
                sub_08003B44(p, p->commands[i][1]);
                break;
            case 1:
                sub_08003B84(p, p->commands[i][1]);
                break;
        }
    }
    p->count = 0;
    used = 0;
    out = p->drawables;
    entry = p->entries;
    for (j = 0; j < p->actorCount; j++, entry++) {
        struct TerrainOverlayActor *actor = entry->actor;
        s32 quadrant, yFixed, y, x, z;
        if (!entry->used)
            continue;
        if (actor->parent && (actor->parent->flags & 1))
            continue;
        if (actor->state && (*actor->state & 1))
            continue;
        {
            u32 mask = 1;
            if (mask & actor->flags)
                continue;
        }
        yFixed = actor->y & 0xFF00;
        x = actor->x >> 8;
        z = actor->z >> 8;
        y = yFixed >> 8;
        heights[0] = y;
        tiles[0] = 255;
        if ((actor->x & 255) <= 63) {
            if ((actor->z & 255) <= 63) {
                xs[0] = x + 1;
                zs[0] = z;
                xs[1] = x + 1;
                zs[1] = z + 1;
                xs[2] = x;
                zs[2] = z + 1;
                quadrant = 0;
            } else {
                xs[0] = x - 1;
                zs[0] = z + 1;
                xs[1] = x;
                zs[1] = z + 1;
                xs[2] = x + 1;
                zs[2] = z + 1;
                quadrant = 1;
            }
        } else {
            if ((actor->z & 255) <= 63) {
                xs[0] = x + 1;
                zs[0] = z - 1;
                xs[1] = x + 1;
                zs[1] = z;
                xs[2] = x + 1;
                zs[2] = z + 1;
                quadrant = 2;
            } else {
                xs[0] = x + 1;
                zs[0] = z;
                xs[1] = x + 1;
                zs[1] = z + 1;
                xs[2] = x;
                zs[2] = z + 1;
                quadrant = 3;
            }
        }
        for (i = 0; i < 3; i++) {
            struct TerrainOverlayCell *cell;
            u8 *dynamic;
            s32 index = rowOffset(zs[i]) + xs[i];
            indices[i] = index;
            dynamic = sub_0821D64C(index, 1);
            if (dynamic)
                cell = (struct TerrainOverlayCell *)(dynamic + 4);
            else
                cell = (struct TerrainOverlayCell *)(gUnk_030052F4->cells + 12 + index * 4);
            if (!cell || (cell->flags & 16)) {
                heights[i + 1] = 0;
                tiles[i + 1] = 255;
            } else {
                s32 height = cell->height & 15;
                s32 delta;
                heights[i + 1] = height;
                delta = height - heights[0];
                if (delta < 0)
                    delta = -delta;
                if (delta > 1) {
                    struct TerrainOverlayCell *base = baseCell(indices[i]);
                    if (!base || (base->flags & 16))
                        tiles[i + 1] = 255;
                    else if ((base->height & 15) <= heights[0])
                        tiles[i + 1] = cell->tile;
                    else
                        tiles[i + 1] = base->tile;
                } else
                    tiles[i + 1] = cell->tile;
            }
        }
        for (i = 0; i < 3; i++) {
            u32 key, *slot;
            s32 k;
            if (heights[0] >= heights[i + 1] || tiles[i + 1] == 255)
                continue;
            if (quadrant == 1 && heights[0] >= heights[2])
                continue;
            if (quadrant == 2 && heights[0] >= heights[2])
                continue;
            key = indices[i] | (y << 16);
            slot = p->keys;
            for (k = 0; k < used; k++, slot++)
                if (*slot == key)
                    break;
            if (k != used)
                continue;
            out->tile = tiles[i + 1];
            out->x = (xs[i] << 8) + 128;
            out->y = yFixed;
            out->z = (zs[i] << 8) + 128;
            out->flags &= ~1;
            *slot = key;
            used++;
            out++;
        }
    }
    for (; used < p->drawableCount; out++, used++)
        out->flags |= 1;
    return 0;
}
