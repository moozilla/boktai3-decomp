#include "engine/shared/global.h"
#include "engine/shared/display_keyframe.h"
#include "engine/shared/scene_actor_components.h"
#include "engine/interrupts_and_video/sprite.h"
#include "engine/shared/terrain_collision.h"
#include "field/terrain_overlays/terrain_overlay.h"

struct SceneActorBody *SceneBody_GetById(u16);

void TerrainOverlay_AddActor(struct TerrainOverlay *s, u16 id)
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
        e->actor = SceneBody_GetById(id);
        if (e->actor != 0) {
            e->used = 1;
            e->id = id;
        }
    }
}

void TerrainOverlay_RemoveActor(struct TerrainOverlay *s, u16 id)
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

struct TerrainOverlay *ResourceRegistry_GetResource(u16);

s32 TerrainOverlay_QueueCommand(u32 type, u32 id)
{
    struct TerrainOverlay *p = ResourceRegistry_GetResource(0x7BE3);
    if (!p)
        return -1;
    if (p->count > 15)
        return -1;
    p->commands[p->count][0] = type;
    p->commands[p->count][1] = id;
    p->count++;
    return 0;
}

u32 TerrainOverlay_QueueAddActor(u32 *p)
{
    return TerrainOverlay_QueueCommand(0, *p);
}

u32 TerrainOverlay_QueueRemoveActor(u32 *p)
{
    return TerrainOverlay_QueueCommand(1, *p);
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);

s32 Sprite_LinkToList(struct DisplaySequenceSprite *, u32);

static inline struct TerrainCell *baseCell(s32 index)
{
    struct TerrainCell *cell;
    if (!gTerrainMap->cells)
        return 0;
    cell = &((struct TerrainGrid *)gTerrainMap->cells)->cells[index];
    return cell;
}

static inline u16 rowOffset(s32 row)
{
    return gTerrainMap->rows[row];
}

static inline s32 SceneBody_TestFlags(const struct SceneActorBody *body, u32 mask)
{
    if (body->flags & mask)
        return 1;
    return 0;
}

s32 TerrainOverlay_Update(struct TerrainOverlay *p)
{
    s32 heights[4], tiles[4], xs[3], zs[3], indices[3];
    s32 i, j, used;
    struct DisplaySequenceSprite *out;
    struct TerrainOverlayEntry *entry;
    if (!p->initialized) {
        p->initialized = 1;
        for (i = 0; i < p->drawableCount; i++) {
            struct DisplaySequenceSprite *base = p->drawables;
            struct DisplaySequenceSprite *d =
                (struct DisplaySequenceSprite *)(sizeof(*base) * i + (u32)base);
            if (!d->unknown4[0])
                Sprite_LinkToList(d, 0);
        }
    }
    for (i = 0; (u32)i < p->count; i++) {
        switch (p->commands[i][0]) {
            case 0:
                TerrainOverlay_AddActor(p, p->commands[i][1]);
                break;
            case 1:
                TerrainOverlay_RemoveActor(p, p->commands[i][1]);
                break;
        }
    }
    p->count = 0;
    used = 0;
    out = p->drawables;
    entry = p->entries;
    for (j = 0; j < p->actorCount; j++, entry++) {
        const struct SceneActorBody *actor = entry->actor;
        s32 quadrant, yFixed, y, x, z;
        if (!entry->used)
            continue;
        if (actor->keyframeDisplay && (actor->keyframeDisplay->flags & 1))
            continue;
        {
            const struct DisplaySequenceSprite *sprite = actor->sprite;
            if (sprite && (sprite->flags & 1))
                continue;
        }
        if (SceneBody_TestFlags(actor, 1))
            continue;
        yFixed = actor->position.y & 0xFF00;
        x = actor->position.x >> 8;
        z = actor->position.z >> 8;
        y = yFixed >> 8;
        heights[0] = y;
        tiles[0] = 255;
        if ((actor->position.x & 255) <= 63) {
            if ((actor->position.z & 255) <= 63) {
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
            if ((actor->position.z & 255) <= 63) {
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
            struct TerrainCell *cell;
            u8 *dynamic;
            s32 index = rowOffset(zs[i]) + xs[i];
            indices[i] = index;
            dynamic = TerrainModifiers_FindCellOverride(index, 1);
            if (dynamic)
                cell = &((struct TerrainCellOverride *)dynamic)->cell;
            else
                cell = &((struct TerrainGrid *)gTerrainMap->cells)->cells[index];
            if (!cell || (cell->flags & 16)) {
                heights[i + 1] = 0;
                tiles[i + 1] = 255;
            } else {
                s32 height = cell->shapeAndHeight & 15;
                s32 delta;
                heights[i + 1] = height;
                delta = height - heights[0];
                if (delta < 0)
                    delta = -delta;
                if (delta > 1) {
                    struct TerrainCell *base = baseCell(indices[i]);
                    if (!base || (base->flags & 16))
                        tiles[i + 1] = 255;
                    else if ((base->shapeAndHeight & 15) <= heights[0])
                        tiles[i + 1] = cell->attribute;
                    else
                        tiles[i + 1] = base->attribute;
                } else
                    tiles[i + 1] = cell->attribute;
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
            out->frame = tiles[i + 1];
            out->position.x = (xs[i] << 8) + 128;
            out->position.y = yFixed;
            out->position.z = (zs[i] << 8) + 128;
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

s32 TerrainOverlay_DestroyDrawables(struct TerrainOverlay *p)
{
    s32 i;
    i = 0;
    goto test;
loop: {
    struct DisplaySequenceSprite *base = p->drawables;
    struct DisplaySequenceSprite *d =
        (struct DisplaySequenceSprite *)(sizeof(*base) * i + (u32)base);
    if (d->unknown4[0])
        Sprite_Remove(d);
}
    i++;
test:
    if (i < p->drawableCount)
        goto loop;
    return 0;
}

extern const u8 gUnk_0824DAFC[];

u16 String_Hash(const u8 *);

void Sprite_InitUnlinked(struct DisplaySequenceSprite *, const u8 *, u32);

void Video_SetBlendControl(s32, s32, s32);

void Video_SetBlendAlpha(s32, s32);

void ResourceRegistry_RegisterResource(u16, void *, s32);

s32 TerrainOverlay_Initialize(struct TerrainOverlay *p, u32 id, u32 resource)
{
    s32 i;
    s32 result;
    p->id = id;
    p->resource = resource;
    p->initialized = 0;
    p->count = 0;
    if (Script_SeekToKeyword('m')) {
        s32 key = Script_GetValue();
        result = AnimData_Load(p->model, key);
    } else
        result = AnimData_Load(p->model, String_Hash(gUnk_0824DAFC));
    if (!result)
        return -1;
    for (i = 0; i < p->drawableCount; i++) {
        struct DisplaySequenceSprite *d = &p->drawables[i];
        Sprite_InitUnlinked(d, p->model, 1);
        d->attributes |= 0x400;
    }
    Video_SetBlendControl(2, 12, 44);
    Video_SetBlendAlpha(0, 16);
    ResourceRegistry_RegisterResource(0x7BE3, p, 0);
    return 0;
}

void *TerrainOverlay_CreateFromScript(u16 x, u16 y)
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
    s = Task_Create(9, a + b + n * 12);
    if (s != 0) {
        Task_SetCallbacks(s, TerrainOverlay_Update, TerrainOverlay_DestroyDrawables);
        s->drawables = (struct DisplaySequenceSprite *)((u8 *)s + b);
        s->keys = (u32 *)((u8 *)s->drawables + a);
        s->actorCount = n;
        s->drawableCount = n * 3;
        if (TerrainOverlay_Initialize(s, x, y) < 0) {
            Task_Kill(s);
            return 0;
        }
    }
    return s;
}

s32 Centipede_Create(void)
{
    s32 a = Script_GetKeywordValue(0x6d, 0);
    s32 b = Script_GetKeywordValue(0x6e, 0);
    if (b == 0)
        return -1;
    return TerrainOverlay_QueueCommand(a, b);
}
