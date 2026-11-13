#define ObjGfx_UploadQueuedTiles        sub_0821537C
#define ObjGfx_AllocateTileSlot         sub_082152E4
#define Sprite_RenderCameraRelativeList sub_08214D20
#define Sprite_QueueFramePieces         sub_08214600
#define Sprite_SetAnimDataIfPresent     sub_082145F4
#define Anim_SetCurrentListHead         sub_0821435C
#define Effect_SetCurrentListHead       sub_08214344
#define Sprite_SetCurrentListHead       sub_0821432C
#include "sprite.h"
#include "sprite_oam.h"
#include "scene_input_widget.h"
#include "interrupts_video.h"
#include "global.h"
#include "render.h"
#include "runtime.h"
#include "ram.h"
#include "gba/gba.h"
#include "obj_tile_upload.h"

void Sprite_ApplyAnimation(struct DisplaySequenceSprite *, const u8 *);
s32 Sprite_LinkToList(struct DisplaySequenceSprite *, u32);
void Sprite_UnlinkFromList(struct DisplaySequenceSprite *, u32);
void SpriteOam_ResetQueue(s32);

void ObjGfx_ResetAllocationCounters(void);

void SpriteOam_ResetQueue(s32);

void Sprite_InitSystem(void)
{
    s32 i;
    u32 zero;
    void **spriteHeads, **effectHeads, **widgetHeads;
    gCamera.position.x = 0;
    gCamera.position.y = 0;
    gCamera.position.z = 0;
    gCamera.unk0C = 1;
    gCamera.unk10 = 0;
    gRenderListIndex = 0;
    zero = 0;
    widgetHeads = gMainSpriteListHeads;
    effectHeads = gDrawableListHeads;
    spriteHeads = gSpriteListHeads;
    for (i = 1; i >= 0; i--) {
        *spriteHeads++ = zero;
        *effectHeads++ = zero;
        *widgetHeads++ = zero;
    }
    ObjGfx_ResetAllocationCounters();
    SpriteOam_ResetQueue(1);
}

/* Sprite_SetCurrentListHead: Stores a sprite list head in the currently selected list slot */

void Sprite_SetCurrentListHead(void *head)
{
    gSpriteListHeads[gRenderListIndex] = head;
}

/* Effect_SetCurrentListHead: Sets the effect list head for the current list index. */

void Effect_SetCurrentListHead(void *head)
{
    gDrawableListHeads[gRenderListIndex] = head;
}

/* Anim_SetCurrentListHead: Sets the animation list head for the current list index. */

void Anim_SetCurrentListHead(void *head)
{
    gMainSpriteListHeads[gRenderListIndex] = head;
}

s32 Sprite_LinkToList(struct DisplaySequenceSprite *entry, u32 list)
{
    struct DisplaySequenceSprite *head;
    entry->unknownA[0] = list;
    entry->unknown4[0] = 1;
    entry->previous = 0;
    head = gSpriteListHeads[list];
    entry->next = head;
    if (head)
        head->previous = entry;
    gSpriteListHeads[list] = entry;
    return 0;
}

void Sprite_UnlinkFromList(struct DisplaySequenceSprite *entry, u32 list)
{
    struct DisplaySequenceSprite *previous = entry->previous;
    struct DisplaySequenceSprite *next = entry->next;
    entry->unknown4[0] = 0;
    if (previous)
        previous->next = next;
    else
        gSpriteListHeads[list] = next;
    if (next)
        next->previous = previous;
}

s32 Effect_LinkToList(struct SpriteDrawable *entry, u32 list)
{
    struct SpriteDrawable *head;
    entry->list = list;
    entry->linked = 1;
    entry->previous = 0;
    head = gDrawableListHeads[list];
    entry->next = head;
    if (head)
        head->previous = entry;
    gDrawableListHeads[list] = entry;
    return 0;
}

void Effect_UnlinkFromList(struct SpriteDrawable *entry, u32 list)
{
    struct SpriteDrawable *previous = entry->previous;
    struct SpriteDrawable *next = entry->next;
    entry->linked = 0;
    if (previous)
        previous->next = next;
    else
        gDrawableListHeads[list] = next;
    if (next)
        next->previous = previous;
}

s32 Anim_LinkToList(struct SceneInputWidget *entry, u32 list)
{
    struct SceneInputWidget *head;
    entry->list = list;
    entry->active = 1;
    entry->previous = 0;
    head = gMainSpriteListHeads[list];
    entry->next = head;
    if (head)
        head->previous = entry;
    gMainSpriteListHeads[list] = entry;
    return 0;
}

void Anim_UnlinkFromList(struct SceneInputWidget *entry, u32 list)
{
    struct SceneInputWidget *previous = entry->previous;
    struct SceneInputWidget *next = entry->next;
    entry->active = 0;
    if (previous)
        previous->next = next;
    else
        gMainSpriteListHeads[list] = next;
    if (next)
        next->previous = previous;
}

void Video_SetRenderCallbacks(u16 mode, u32 projected, u32 sprites, u32 extra)
{
    gCamera.mode = mode;
    gProjectedSpriteCallback = projected;
    gSpriteRenderCallback = sprites;
    gExtraRenderCallback = extra;
}

void Sprite_Init(struct DisplaySequenceSprite *sprite, const u8 *resource, u32 flags)
{
    if (sprite->unknown4[0] == 0) {
        sprite->flags = flags;
        sprite->unknown4[1] = 1;
        sprite->attributes = 0;
        sprite->palette = 0x40;
        sprite->paletteVariant = 0x40;
        sprite->attribute = 0;
        sprite->priority = 2;
        Sprite_ApplyAnimation(sprite, resource);
        {
            u32 list = (u32) - (flags & 0x80) >> 31;
            sprite->previous = 0;
            sprite->next = 0;
            Sprite_LinkToList(sprite, list);
        }
    }
}

void Sprite_InitUnlinked(struct DisplaySequenceSprite *sprite, const u8 *resource, u32 flags)
{
    if (sprite->unknown4[0] == 0) {
        sprite->flags = flags;
        sprite->unknown4[1] = 1;
        sprite->attributes = 0;
        sprite->palette = 0x40;
        sprite->paletteVariant = 0x40;
        sprite->attribute = 0;
        sprite->priority = 2;
        Sprite_ApplyAnimation(sprite, resource);
        sprite->previous = 0;
        sprite->next = 0;
    }
}

void Sprite_UnlinkFromList(struct DisplaySequenceSprite *, u32);

void Sprite_Remove(struct DisplaySequenceSprite *sprite)
{
    if (sprite->unknown4[0] != 0)
        Sprite_UnlinkFromList(sprite, sprite->unknownA[0]);
}

void Sprite_CopyRenderState(struct DisplaySequenceSprite *destination,
                            struct DisplaySequenceSprite *source,
                            struct SpriteRenderResource *destinationResource,
                            struct SpriteRenderResource *sourceResource)
{
    u8 visible;
    *destinationResource = *sourceResource;
    destination->flags = source->flags;
    visible = source->unknown4[1];
    destination->attribute = source->attribute;
    destination->priority = source->priority;
    destination->palette = source->palette;
    destination->paletteVariant = source->paletteVariant;
    destination->unknown4[1] = visible;
    destination->resource = (const u8 *)destinationResource;
    /* The public sprite keeps these frame fields flat for sequence consumers. */
    *((struct SpriteRenderAnimationState *)&destination->frame) =
        *((struct SpriteRenderAnimationState *)&source->frame);
    destination->position = source->position;
}

void Sprite_ResetRenderDefaults(struct DisplaySequenceSprite *sprite)
{
    sprite->flags = 0;
    sprite->unknown4[1] = 1;
    sprite->attributes = 0;
    sprite->palette = 0x40;
    sprite->paletteVariant = 0x40;
    sprite->attribute = 0;
    sprite->priority = 2;
}

void Sprite_SetOriginStub(void)
{
}

void Sprite_ApplyAnimation(struct DisplaySequenceSprite *sprite, const u8 *resource)
{
    if (resource) {
        sprite->frame = 0;
        sprite->frameIndex = 0;
        sprite->elapsed = 0;
        sprite->width = gSpriteWidthPixels[resource[1]];
        sprite->height = gSpriteHeightPixels[resource[1]];
        sprite->offsetX = 0;
        sprite->offsetY = 0;
        sprite->attributes = (sprite->attributes & 0x1c00) |
                             (((resource[1] & 3) << 14) | ((resource[1] & 12) << 28));
        if (resource[0] & 0x10)
            sprite->attributes |= 0x2000;
        sprite->resource = resource;
    }
}

/* Sprite_SetAnimDataIfPresent: Replaces the sprite data pointer at offset 0xC only when the supplied pointer is nonzero. */

void Sprite_SetAnimDataIfPresent(struct DisplaySequenceSprite *sprite, const u8 *resource)
{
    if (resource)
        sprite->resource = resource;
}

extern const s16 gUnk_086149C4[];

s32 ObjGfx_AllocateTileSlot(const void *src, s32 size);

u32 Palette_CacheObjectBank(u32 key, void *src);

static inline s32 Sprite_DivideScale(s32 v)
{
    return v >= 0 ? v >> 6 : -((-v) >> 6);
}

static inline s32 Sprite_DivideTrigScale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}

static inline void Oam_PushImmediate(u32 a, u32 t)
{
    s32 n = gOamCount;
    if (n <= 127) {
        gOamBuffer[n].attributes = a;
        gOamBuffer[n].tile = t;
        gOamCount = n + 1;
        gVideoUploadFlags |= 1;
    }
}

static inline void Oam_PushSorted(u32 a, u32 t, s32 z, u32 priority)
{
    s32 n = gPendingOamCount;
    if (n <= 127) {
        struct OamEntry *e = &gPendingOamBuffer[n];
        e->attributes = a;
        e->tile = t;
        e->depth = ((z + 128) & 0xFF) << 8;
        gPendingOamCount = n + 1;
        gOamPriorityCounts[priority]++;
    }
}

static inline s32 Sprite_AllocateAffine(u8 rotation, s8 sx, s8 sy)
{
    s32 n;
    if (gAffineRequestCount > 31)
        return 0;
    gSpriteAffineParams[gAffineRequestCount].angle = rotation;
    gSpriteAffineParams[gAffineRequestCount].scaleX = sx;
    gSpriteAffineParams[gAffineRequestCount].scaleY = sy;
    n = gAffineRequestCount++;
    return n;
}

// Culls one sprite's frame pieces against the Sprite_ProjectPosition, allocates its affine matrix,

// tiles and palette, and queues the visible pieces as OAM entries (immediate or by priority).

void Sprite_QueueFramePieces(struct DisplaySequenceSprite *p, s32 x, s32 y, s32 z)
{
    u32 attrs[32];
    s32 tiles[32];
    s32 k;
    s32 sinv, cosv;
    u32 attr;
    s32 count;
    struct SpriteRenderResource *res;
    s32 tile;
    u8 prev;
    s32 baseX, baseY;
    u32 priority;
    struct SpriteRenderAnimationState *anim;
    struct SpriteRenderFrame *frame;
    struct SpriteRenderPiece *piece;

    z = z >= 0 ? z >> 1 : -((-z) >> 1);
    if (z < -128)
        z = -128;
    else if (z > 127)
        z = 127;
    attr = 0;
    if (((gSpriteMosaicAttr & 1) && p->priority != 0 && !(p->flags & 0x10000)) ||
        (p->flags & 0x800))
        attr |= 0x1000;
    if (p->flags & 0x100)
        attr |= 0x800;
    if (gSpriteAttrFlags != 0 && (p->flags & 0x4000))
        attr |= 0x400;
    attr |= p->attributes;
    if (!(p->flags & 2)) {
        if (p->flags & 4)
            attr |= 0x10000000;
        if (p->flags & 8)
            attr |= 0x20000000;
        sinv = 0;
        cosv = 0;
    } else {
        s32 sx = ((s8)p->palette);
        s32 sy = ((s8)p->paletteVariant);
        if (p->flags & 4)
            sx = -sx;
        if (p->flags & 8)
            sy = -sy;
        if (p->flags & 0x20000)
            attr |= 0x100;
        else if ((sx >= 0 ? sx : -sx) > 60 || (sy >= 0 ? sy : -sy) > 60 || p->attribute != 0)
            attr |= 0x300;
        else
            attr |= 0x100;
        attr |= Sprite_AllocateAffine(p->attribute, sx, sy) << 25;
        sinv = gUnk_086149C4[-p->attribute & 0xFF];
        cosv = gUnk_086149C4[(-p->attribute + 0x40) & 0xFF];
    }
    res = (struct SpriteRenderResource *)p->resource;
    anim = (struct SpriteRenderAnimationState *)&p->frame;
    frame = res->frames;
    frame += anim->frame;
    {
        u8 *pieces = gAnimResourceSection14 + frame->pieceOffset;
        if (p->flags & 4)
            x -= anim->x;
        else
            x += anim->x;
        baseX = x;
        if (p->flags & 8)
            baseY = y - anim->y;
        else
            baseY = y + anim->y;
        priority = p->priority;
        count = 0;
        tile = 0;
        prev = 0;
        piece = (struct SpriteRenderPiece *)pieces;
    }
    for (k = 0; k < frame->count; k++, piece++) {
        u32 shape = piece->shape;
        s32 w, h, left, top;
        tile += prev;
        w = gSpriteWidthPixels[shape];
        h = gSpriteHeightPixels[shape];
        prev = gSpriteTileArea[shape];
        if (!(p->flags & 2)) {
            s32 right, bottom;
            if (p->flags & 4) {
                right = -piece->x;
                left = right - w;
            } else {
                left = piece->x;
                right = left + w;
            }
            if (p->flags & 8) {
                bottom = -piece->y;
                top = bottom - h;
            } else {
                top = piece->y;
                bottom = top + h;
            }
            left += baseX;
            top += baseY;
            if (left > 240 || top > 160 || right + baseX < 0 || bottom + baseY < 0)
                continue;
        } else {
            s32 hw = w >> 1;
            s32 hh = h >> 1;
            s32 dx, dy, v;
            v = (piece->x + hw) * ((s8)p->palette);
            dx = Sprite_DivideScale(v);
            v = (piece->y + hh) * ((s8)p->paletteVariant);
            dy = Sprite_DivideScale(v);
            if (p->flags & 4)
                dx = -dx;
            if (p->flags & 8)
                dy = -dy;
            v = dx * cosv - dy * sinv;
            left = Sprite_DivideTrigScale(v) - hw;
            v = dx * sinv + dy * cosv;
            top = Sprite_DivideTrigScale(v) - hh;
            if ((attr & 0x300) == 0x300) {
                left -= hw;
                top -= hh;
                w <<= 1;
                h <<= 1;
            }
            left += baseX;
            top += baseY;
            if (left + w < 0 || left > 240 || top + h < 0 || top > 160)
                continue;
        }
        attrs[count] = ((left & 0x1FF) << 16) | (top & 0xFF) | attr | gSpriteShapeAttr[shape];
        tiles[count] = tile;
        count++;
    }
    if (count > 0) {
        s32 base =
            ObjGfx_AllocateTileSlot(gAnimResourceSection0C + frame->tileOffset, frame->tileSize);
        u32 extra =
            (priority << 10) |
            (Palette_CacheObjectBank(res->paletteKey + anim->paletteOffset, res->palette) << 12);
        if (p->flags & 0x20) {
            for (k = 0; k < count; k++)
                Oam_PushImmediate(attrs[k], (base + tiles[k]) | extra);
        } else {
            for (k = 0; k < count; k++)
                Oam_PushSorted(attrs[k], (base + tiles[k]) | extra, z, priority);
        }
    }
}

void Sprite_QueueFramePieces(struct DisplaySequenceSprite *, s32, s32, s32);

static inline s32 Sprite_ProjectCoordinate(s32 input)
{
    s32 v = input * 3;
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}

static inline void Sprite_ProjectPosition(struct ContactPosition *dst, struct ContactPosition *src)
{
    s32 depth, height, y, z;
    s32 sx = src->x, sz = src->z;
    dst->x = Sprite_ProjectCoordinate(sx - sz);
    depth = Sprite_ProjectCoordinate(sx + sz);
    height = Sprite_ProjectCoordinate(src->y);
    y = depth - height;
    z = depth + height;
    dst->x = dst->x - gCameraOrigin[0] + 120;
    dst->y = y - gCameraOrigin[1] + 80;
    dst->z = z - gCameraOrigin[2];
}

// Applies signed sprite scales and flip flags when testing resource bounds.

void Sprite_RenderScaledList(void)
{

    s32 z = 0;
    struct ContactPosition pos;
    struct DisplaySequenceSprite *p;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity) {
        skip = 0x201;
    }
    do {
        p = gSpriteListHeads[gRenderListIndex];
        if (!p) {
            return;
        }
    } while (0);
    for (; p; p = p->next) {
        s32 x;
        s32 y;
        struct SpriteRenderResource *resource;
        if (p->flags & skip) {
            continue;
        }
        resource = (struct SpriteRenderResource *)p->resource;
        if (!(p->flags & 16)) {
            Sprite_ProjectPosition(&pos, &p->position);
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = p->position.x;
            y = p->position.y;
            z = p->position.z;
        }
        if (!(p->flags & 64)) {
            s32 left, top;
            u32 width, height;
            if (p->flags & 2) {
                s32 ox = (resource->offsetX * ((s8)p->palette)) >> 6;
                s32 oy = (resource->offsetY * ((s8)p->paletteVariant)) >> 6;
                width = (resource->width * ((s8)p->palette)) >> 6;
                height = (resource->height * ((s8)p->paletteVariant)) >> 6;
                if (p->flags & 4) {
                    left = x + ox - width;
                } else {
                    left = x - ox;
                }
                if (p->flags & 8) {
                    top = y + oy - height;
                } else {
                    top = y - oy;
                }
            } else {
                width = resource->width;
                height = resource->height;
                if (p->flags & 4) {
                    left = x + resource->offsetX - width;
                } else {
                    left = x - resource->offsetX;
                }
                if (p->flags & 8) {
                    top = y + resource->offsetY - height;
                } else {
                    top = y - resource->offsetY;
                }
            }
            if ((((left > 240) || (top > 160)) || ((left + ((s32)width)) < 0)) ||
                ((top + ((s32)height)) < 0)) {
                continue;
            }
        }
        Sprite_QueueFramePieces(p, x, y, z);
    }
}

// Sprite list cull/draw, unscaled variant of Sprite_RenderScaledList.

static inline s32 Sprite_DividePosition(s32 v)
{
    return v >= 0 ? v >> 3 : -((-v) >> 3);
}

void Sprite_RenderCameraRelativeList(void)
{
    struct DisplaySequenceSprite *p;
    s32 z;
    u32 width, height;
    s32 left, top;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity) {
        skip = 0x201;
    }
    do {
        do {
            p = gSpriteListHeads[gRenderListIndex];
            if (!p) {
                return;
            }
        } while (0);
    } while (0);
    for (; p; p = p->next) {
        s32 x;
        s32 y;
        struct SpriteRenderResource *resource;
        if (p->flags & skip) {
            continue;
        }
        resource = (struct SpriteRenderResource *)p->resource;
        x = Sprite_DividePosition(p->position.x - gCamera.position.x) + 120;
        y = Sprite_DividePosition(p->position.y - gCamera.position.y) + 80;
        z = p->position.z - gCamera.position.z;
        if (!(p->flags & 64)) {
            if (p->flags & 4) {
                left = x + resource->offsetX - resource->width;
                width = resource->width;
            } else {
                left = x - resource->offsetX;
                width = resource->width;
            }
            if (p->flags & 8) {
                top = y + resource->offsetY - resource->height;
                height = resource->height;
            } else {
                top = y - resource->offsetY;
                height = resource->height;
            }
            if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0) {
                continue;
            }
        }
        Sprite_QueueFramePieces(p, x, y, z);
    }
}

// confidence=low: resource flag selects this list; name describes observed bounds handling.

void Sprite_RenderFlipAwareList(void)
{
    struct ContactPosition pos;
    struct DisplaySequenceSprite *p;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity) {
        skip = 0x201;
    }
    p = gSpriteListHeads[gRenderListIndex];
    if (!p) {
        return;
    }
    for (; p; p = p->next) {
        s32 x, y, z;
        struct SpriteRenderResource *resource;
        if (p->flags & skip) {
            continue;
        }
        if (!(p->flags & 0x1000)) {
            continue;
        }
        resource = (struct SpriteRenderResource *)p->resource;
        if (!(p->flags & 16)) {
            Sprite_ProjectPosition(&pos, &p->position);
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = p->position.x;
            y = p->position.y;
            z = p->position.z;
        }
        if (!(p->flags & 64)) {
            s32 left;
            s32 top;
            u32 width;
            u32 height;
            s32 bot;
            if (p->flags & 4) {
                s32 edge = x + resource->offsetX;
                left = edge - resource->width;
            } else {
                left = x - resource->offsetX;
            }
            width = resource->width;
            if (p->flags & 8) {
                s32 t = y + resource->offsetY;
                top = t - resource->height;
                height = resource->height;
            } else {
                short t = resource->offsetY;
                top = y - t;
                height = resource->height;
            }
            if ((((left > 240) || (top > 160)) || ((left + ((s32)width)) < 0)) ||
                ((bot = top + ((s32)height)) < 0)) {
                continue;
            }
        }
        Sprite_QueueFramePieces(p, x, y, z);
    }
}

// confidence=low: resource flag selects this list; name describes observed bounds handling.

void Sprite_RenderUnflippedList(void)
{
    struct ContactPosition pos;
    struct DisplaySequenceSprite *p;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity)
        skip = 0x201;
    p = gSpriteListHeads[gRenderListIndex];
    if (!p)
        return;
    for (; p; p = p->next) {
        s32 x, y, z;
        struct SpriteRenderResource *resource;
        if (p->flags & skip)
            continue;
        if (!(p->flags & 0x2000))
            continue;
        resource = (struct SpriteRenderResource *)p->resource;
        if (!(p->flags & 16)) {
            Sprite_ProjectPosition(&pos, &p->position);
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = p->position.x;
            y = p->position.y;
            z = p->position.z;
        }
        if (!(p->flags & 64)) {
            s32 left, top;
            u32 width, height;
            left = x - resource->offsetX;
            top = y - resource->offsetY;
            width = resource->width;
            height = resource->height;
            if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0)
                continue;
        }
        Sprite_QueueFramePieces(p, x, y, z);
    }
}

void Video_OnRenderNoOp(void)
{
}

u16 *ScriptProc_Start(u32, u32);

void sub_082171C0(u16 *, u32);

void AnimData_InitPalettePool(void)
{
    u16 *p = ScriptProc_Start(0x9B1B, 0xC5E9);
    gPaletteBankCount = p[0];
    gPaletteBankData = (u8 *)(p + 2);
    sub_082171C0(p + 2, 8);
}

void SpriteTiles_LoadResource(struct SpriteTileResource *resource)
{
    gSpriteTileResource = resource;
    gStaticTileCount = resource->tileCount;
    {
        struct VideoDmaRegisters *dma = (struct VideoDmaRegisters *)REG_ADDR_DMA3SAD;
        dma->source = (u32)resource + resource->dataOffset;
        dma->destination = OBJ_VRAM0;
        dma->control = ((resource->tileCount << 5) / 4) | 0x84000000;
        dma->control;
    }
}

struct SpriteTileEntry *AnimData_FindEntry(u16 id)
{
    struct SpriteTileResource *h = gSpriteTileResource;
    if (h == 0)
        return 0;
    {
        struct SpriteTileEntry *e = h->entries;
        s32 i = 0;
        for (; i < h->entryCount; i++, e++) {
            if (e->id == id)
                return e;
        }
    }
    return 0;
}

void AnimData_SetResourceBank(struct AnimResourceHeader *h)
{
    gAnimationResource = h;
    gAnimResourceSection14 = (u8 *)h + h->offset14;
    gAnimResourceSection0C = (u8 *)h + h->offsetC;
}

/* Sprite resource bank (header at 0x03004300): entries of 12 bytes from +0x18. */

/* Fills anim from the resource entry with this id; 0 if there is none. */

s32 AnimData_Load(struct SpriteRenderResource *anim, u32 id)
{
    struct AnimResourceHeader *header = gAnimationResource;
    u8 *base10, *baseC;
    struct AnimResourceEntry *entry;
    u16 *info;
    s32 found, i;
    if (!header)
        return 0;
    baseC = (u8 *)header + header->offsetC;
    base10 = (u8 *)header + header->offset10;
    found = 0;
    entry = header->entries;
    for (i = 0; i < gAnimationResource->count; entry++, i++) {
        if (entry->id == id) {
            found = 1;
            break;
        }
    }
    if (!found)
        return 0;
    info = (u16 *)(entry->offset + base10);
    anim->flags = entry->field2;
    anim->shape = 0;
    anim->unknown2 = 0;
    anim->width = entry->field4;
    anim->height = entry->field5;
    anim->offsetX = entry->field6;
    anim->offsetY = entry->field7;
    anim->unknown4 = 0;
    anim->paletteKey = info[1];
    anim->palette = gPaletteBankData + anim->paletteKey * 32;
    anim->tileData = ((u32 *)info)[1] + baseC;
    anim->frames = (struct SpriteRenderFrame *)(entry->offset + base10);
    return 1;
}

void AnimData_SelectPaletteBank(struct SpriteRenderResource *resource, s32 index)
{
    if (index < gPaletteBankCount) {
        resource->paletteKey = index;
        resource->palette = gPaletteBankData + (resource->paletteKey << 5);
    }
}

void ObjGfx_ResetAllocationCounters(void)
{
    gDynamicTileSum = 0;
    gMainSpriteTileSum = 0;
    gObjTileCursor = 0;
    if (gSpriteTileResource != 0)
        gObjTileCursor = gStaticTileCount;
}

s32 ObjGfx_AllocateTileSlot(const void *src, s32 size)
{
    struct ObjGfxSlot *slot;
    struct ObjGfxSlot *entry;
    s32 i;

    if (gQueuedObjSlotCount > 0x7F)
        return ((s16)gStaticTileCount);
    if (gObjTileCursor + size > 0x3FF)
        return ((s16)gStaticTileCount);
    for (slot = gObjGfxSlots, i = 0; i < gQueuedObjSlotCount; i++, slot++) {
        if (slot->src == src)
            return slot->offset;
    }
    entry = &gObjGfxSlots[gQueuedObjSlotCount];
    entry->size = size;
    entry->offset = gObjTileCursor;
    entry->src = src;
    gObjTileCursor += size;
    gDynamicTileSum += size;
    gQueuedObjSlotCount++;
    return entry->offset;
}

// Upload queued tile slots to OBJ VRAM using DMA3.

void ObjGfx_LoadTiles(u32 start);

void ObjGfx_UploadQueuedTiles(void)
{
    u32 dest = OBJ_VRAM0 + gStaticTileCount * 32;
    struct ObjGfxSlot *slot = gObjGfxSlots;
    s32 i;
    for (i = 0; i < gQueuedObjSlotCount; i++) {
        s32 bytes = slot->size << 5;
        DmaCopy32(3, slot->src, dest, bytes);
        dest += bytes;
        slot++;
    }
    gQueuedObjSlotCount = 0;
    ObjGfx_LoadTiles(gStaticTileCount + gDynamicTileSum);
}

void ObjGfx_LoadTiles(u32 start)
{
    u32 i;
    struct ObjTileUploadRequest *entry = gObjTileUploadRequests;
    u32 dst = OBJ_VRAM0 + (start << 5);
    u32 src;

    i = 0;
    /* Save the queue length before reusing src for the tile source address. */
    src = gArmTileUploadCount;
    if (i < src) {
        u32 entryCount = src;
        do {
            struct ObjTileUploadRequest *e = entry;
            s32 j;
            src = e->src;
            for (j = 0; j < e->transfer.rows.count; j++) {
                struct VideoDmaRegisters *dma = (struct VideoDmaRegisters *)REG_ADDR_DMA3SAD;
                dma->source = src;
                dma->destination = dst;
                dma->control = (e->transfer.rows.size >> 2) | 0x84000000;
                (void)dma->control;
                src += 0x200;
                dst += e->transfer.rows.size;
            }

            i++;
            entry++;
        } while (i < entryCount);
    }
    gArmTileUploadCount = 0;
}

void SpriteOam_ResetQueue(s32 clear)
{
    u16 *p, *base;
    u32 *count, *affine;
    gOamCount = 0;
    count = &gPendingOamCount;
    affine = &gAffineRequestCount;
    base = gOamPriorityCounts;
    {
        u16 zero = 0;
        p = base + 3;
        do {
            *p-- = zero;
        } while ((s32)p >= (s32)base);
    }
    *count = 0;
    *affine = 0;
    if (clear) {
        struct OamEntry *entry = gOamBuffer;
        u32 hidden = 0x200;
        s32 i = 127;
        do {
            entry->attributes = hidden;
            entry++;
        } while (--i >= 0);
    }
}

void CpuFastSet(const void *, void *, u32);

s32 Div(s32, s32);

void SpriteOam_FinalizeQueue(void)
{
    struct {
        u8 heads[256];
        s32 fill;
    } work;
    s32 priorities[4];
    struct OamEntry *entry;
    s32 i;
    s32 count;
    /* Reused for the OAM buffer and then the affine lookup table. */
    void *base;
    work.fill = -1;
    CpuFastSet(&work.fill, work.heads, 0x1000040);
    {
        entry = gPendingOamBuffer;
        for (i = 0; i < gPendingOamCount; i++) {
            u32 depth = entry->depth;
            u8 *head = &work.heads[depth >> 8];
            entry->depth = depth | (*head);
            *head = i;
            entry++;
        }
    }
    {
        s32 sum;
        sum = gOamCount;
        for (i = 0; i < 4; i++) {
            base = gOamBuffer;
            priorities[i] = sum;
            sum += gOamPriorityCounts[i];
        }
    }
    count = gOamCount;
    for (i = 255; (i >= 0) && (count < 128); i--) {
        s32 index = work.heads[i];
        if (index != 255) {
            do {
                s32 *next;
                struct OamEntry *queued;
                queued = &gPendingOamBuffer[index];
                next = &priorities[(queued->tile >> 10) & 3];
                if ((*next) < 128) {
                    gOamBuffer[*next] = *queued;
                    count++;
                    (*next)++;
                }
                index = (u8)queued->depth;
            } while (index != 255);
        }
    }

    i = count;
    entry = (struct OamEntry *)(i * sizeof(*entry) + (u32)base);
    for (; i < 128; i++, entry++) {
        entry->attributes = 0x200;
    }

    {
        struct SpriteAffineParam *matrix;
        entry = base;
        matrix = gSpriteAffineParams;
        if (gAffineRequestCount > 31)
            gAffineRequestCount = 32;
        for (i = 0; i < gAffineRequestCount; i++) {
            s32 angle;
            s32 sine, cosine, value;
            base = (void *)gUnk_086149C4;
            angle = matrix->angle;
            sine = ((s16 *)base)[angle] * 4;
            cosine = ((s16 *)base)[(angle + 64) & 255] * 4;
            if (!matrix->scaleX) {
                value = matrix->scaleX;
                entry[0].depth = value;
            } else {
                entry[0].depth = Div(cosine, matrix->scaleX);
                value = Div(-sine, matrix->scaleX);
            }
            entry[1].depth = value;
            if (!matrix->scaleY) {
                value = matrix->scaleY;
                entry[2].depth = value;
            } else {
                entry[2].depth = Div(sine, matrix->scaleY);
                value = Div(cosine, matrix->scaleY);
            }
            entry[3].depth = value;
            entry += 4;
            matrix++;
        }
    }
    gVideoUploadFlags |= 1;
}
