#include "global.h"
#include "terrain_overlay.h"

u32 sub_08225984(u16);

void sub_08003B44(struct TerrainOverlay *s, u16 id)
{
    struct TerrainOverlayEntry *e = s->entries;
    s32 i = 0;
    while (i < s->actorCount) {
        if (e->used == 0)
            break;
        e++;
        i++;
    }
    if (i != s->actorCount) {
        e->actor = (struct TerrainOverlayActor *)sub_08225984(id);
        if (e->actor != 0) {
            e->used = 1;
            e->id = id;
        }
    }
}

void sub_08003B84(struct TerrainOverlay *s, u16 id)
{
    struct TerrainOverlayEntry *e = s->entries;
    s32 i;
    for (i = 0; i < s->actorCount; e++, i++) {
        if (e->id == id) {
            e->used = 0;
            e->actor = 0;
            e->id = 0;
            return;
        }
    }
}

struct TerrainOverlay *sub_0821A2C4(u16);

// MATCHING_CONCESSION: register-pinning
s32 TerrainOverlay_QueueCommand(u32 type, u32 id)
{
    struct TerrainOverlay *p = ({
        struct TerrainOverlay *value = sub_0821A2C4(0x7BE3);
        asm("" : "+r"(value));
        value;
    });
    if (p && p->count <= 15) {
        p->commands[p->count][0] = type;
        p->commands[p->count][1] = id;
        p->count++;
        return 0;
    }
    return -1;
}

u32 sub_08003C00(u32 *p)
{
    return TerrainOverlay_QueueCommand(0, *p);
}

u32 sub_08003C10(u32 *p)
{
    return TerrainOverlay_QueueCommand(1, *p);
}

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
            struct TerrainOverlayDrawable *d =
                (struct TerrainOverlayDrawable *)(sizeof(*base) * i + (u32)base);
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

void sub_08214514(struct TerrainOverlayDrawable *);

s32 TerrainOverlay_DestroyDrawables(struct TerrainOverlay *p)
{
    s32 i;
    i = 0;
    goto test;
loop: {
    struct TerrainOverlayDrawable *base = p->drawables;
    struct TerrainOverlayDrawable *d =
        (struct TerrainOverlayDrawable *)(sizeof(*base) * i + (u32)base);
    if (d->active)
        sub_08214514(d);
}
    i++;
test:
    if (i < p->drawableCount)
        goto loop;
    return 0;
}

extern const u8 gUnk_0824DAFC[];

s32 Script_SeekToKeyword(s32);

s32 Script_GetValue(void);

s32 sub_08219BD8(const u8 *);

s32 sub_082151E4(void *, s32);

void sub_082144E4(struct TerrainOverlayDrawable *, void *, s32);

void sub_08217060(s32, s32, s32);

void sub_08217074(s32, s32);

void sub_0821A284(u16, void *, s32);

// MATCHING_CONCESSION: register-pinning
s32 TerrainOverlay_Initialize(struct TerrainOverlay *p, u32 id, u32 resource)
{
    u8 *model;
    u8 *loadedModel;
    s32 key, i;
    p->id = id;
    p->resource = resource;
    p->initialized = 0;
    p->count = 0;
    if (Script_SeekToKeyword('m')) {
        key = Script_GetValue();
        model = p->model;
    } else {
        model = p->model;
        key = ({
            register s32 value asm("r1") = sub_08219BD8(gUnk_0824DAFC);
            asm("" : "+r"(value));
            (u16) value;
        });
    }
    {
        s32 result = sub_082151E4(model, key);
        loadedModel = model;
        if (!result)
            return -1;
    }
    i = 0;
    if (i < p->drawableCount) {
        register struct TerrainOverlayDrawable **drawables asm("r8") = &p->drawables;
        do {
            register u32 offset asm("r0") = sizeof(struct TerrainOverlayDrawable) * i;
            struct TerrainOverlayDrawable *d;
            d = (struct TerrainOverlayDrawable *)((u32)*drawables + offset);
            sub_082144E4(d, loadedModel, 1);
            d->drawFlags |= 0x400;
            i++;
        } while (i < p->drawableCount);
    }
    sub_08217060(2, 12, 44);
    sub_08217074(0, 16);
    sub_0821A284(0x7BE3, p, 0);
    return 0;
}

void *sub_08219FBC(u32, u32);

void sub_0821A04C(void *, s32 (*)(struct TerrainOverlay *), s32 (*)(struct TerrainOverlay *));

void sub_0821A0C0(void *);

void *sub_080040F8(u16 x, u16 y)
{
    struct TerrainOverlay *s;
    s32 n;
    s32 a;
    s32 b;
    if (Script_SeekToKeyword(0x6e))
        n = Script_GetValue();
    else
        n = 8;
    a = n * 0x84;
    b = n * 8 + 0x90;
    s = sub_08219FBC(9, a + b + n * 12);
    if (s != 0) {
        sub_0821A04C(s, TerrainOverlay_Update, TerrainOverlay_DestroyDrawables);
        s->drawables = (struct TerrainOverlayDrawable *)((u8 *)s + b);
        s->keys = (u32 *)((u8 *)s->drawables + a);
        s->actorCount = n;
        s->drawableCount = n * 3;
        if (TerrainOverlay_Initialize(s, x, y) < 0) {
            sub_0821A0C0(s);
            return 0;
        }
    }
    return s;
}

s32 sub_0821ABA8(u32, u32);

s32 sub_08004194(void)
{
    s32 a = sub_0821ABA8(0x6d, 0);
    s32 b = sub_0821ABA8(0x6e, 0);
    if (b == 0)
        return -1;
    return TerrainOverlay_QueueCommand(a, b);
}
