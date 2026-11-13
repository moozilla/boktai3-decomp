#include "sprite_oam.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"

static inline s32 UnsignedScale(u8 v)
{
    return v;
}

static inline s32 SpriteOam_ProjectCoordinate(s32 input)
{
    s32 v = input * 3;
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}

static inline void SpriteOam_ProjectPosition(struct ContactPosition *dst,
                                             struct ContactPosition *src)
{
    s32 depth, height, y, z;
    s32 sx = src->x, sz = src->z;
    dst->x = SpriteOam_ProjectCoordinate(sx - sz);
    depth = SpriteOam_ProjectCoordinate(sx + sz);
    height = SpriteOam_ProjectCoordinate(src->y);
    y = depth - height;
    z = depth + height;
    dst->x = dst->x - gCameraOrigin[0] + 120;
    dst->y = y - gCameraOrigin[1] + 80;
    dst->z = z - gCameraOrigin[2];
}

void SpriteOam_QueueProjectedSprites(void)
{
    struct ContactPosition pos;
    struct SpriteDrawable *sprite;
    s32 *matrixCount;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity)
        skip = 0x201;
    sprite = gDrawableListHeads[gRenderListIndex];
    if (!sprite)
        return;
    matrixCount = &gAffineRequestCount;
    do {
        s32 x, y;
        s32 tmp;
        s32 z;
        u32 flags;
        u32 attr;
        s32 depth;
        s32 ox, oy;
        u32 initialFlags = sprite->flags;
        if (initialFlags & skip) {
            sprite = sprite->next;
            continue;
        }

        if (!(initialFlags & 16)) {
            SpriteOam_ProjectPosition(&pos, &sprite->position);
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = sprite->position.x;
            y = sprite->position.y;
            z = sprite->position.z;
        }
        {
            u32 loaded = sprite->flags;
            u32 cull = loaded & 64;
            flags = loaded;
            ox = sprite->offsetX;
            oy = sprite->offsetY;
            if (!cull) {
                u32 width = sprite->width, height = sprite->height;
                s32 left = x + ox, top = y + oy;
                if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0)
                    goto next;
            }
        }
        x += ox;
        y += oy;
        depth = z;
        x &= 511;
        {
            u32 shifted = x << 16;
            attr = sprite->attributes | shifted;
        }
        y &= 255;
        attr |= y;
        if (flags & 2) {
            u32 matrix;
            u32 angle;
            s32 sx, sy;
            if (!(flags & 0x20000) && (UnsignedScale(sprite->scaleX) > 60 ||
                                       UnsignedScale(sprite->scaleY) > 60 || sprite->angle != 0))
                attr |= 0x300;
            else
                attr |= 0x100;
            angle = sprite->angle;
            sx = (s8)sprite->scaleX;
            sy = (s8)sprite->scaleY;
            if (*matrixCount > 31)
                matrix = 0;
            else {
                s32 *count = matrixCount;
                gSpriteAffineParams[*count].angle = angle;
                gSpriteAffineParams[*count].scaleX = sx;
                gSpriteAffineParams[*count].scaleY = sy;
                {
                    tmp = *count;
                    matrix = tmp++;
                    *count = tmp;
                }
            }
            attr |= matrix << 25;
            flags = sprite->flags;
        }
        if (flags & 4)
            attr |= 0x10000000;
        if (flags & 8)
            attr |= 0x20000000;
        if (gSpriteAttrFlags && (flags & 0x4000))
            attr |= 0x400;
        if (((gSpriteMosaicAttr & 1) && sprite->priority && !(flags & 0x10000)) || (flags & 0x800))
            attr |= 0x1000;
        depth += sprite->depthOffset;
        if (flags & 0x20) {
            s32 tile = sprite->tile | (sprite->priority << 10) | (sprite->palette << 12);
            s32 n = gOamCount;
            if (n > 127)
                goto next;
            gOamBuffer[n].attributes = attr;
            gOamBuffer[n].tile = tile;
            gOamCount = n + 1;
            gVideoUploadFlags |= 1;
        } else {
            s32 tile, n;
            u32 priority;
            tmp = depth >= 0 ? depth >> 1 : -((-depth) >> 1);
            depth = tmp;
            if (depth < -128)
                depth = -128;
            else if (depth > 127)
                depth = 127;
            tile = sprite->tile;
            priority = sprite->priority;
            tile = tile | (priority << 10) | (sprite->palette << 12);
            n = gPendingOamCount;
            if (n > 127)
                goto next;
            gPendingOamBuffer[n].attributes = attr;
            gPendingOamBuffer[n].tile = tile;
            gPendingOamBuffer[n].depth = ((depth + 128) & 255) << 8;
            gPendingOamCount = n + 1;
            gOamPriorityCounts[priority]++;
        }
    next:
        sprite = sprite->next;
    } while (sprite);
}

static inline s32 divide8(s32 input)
{
    return input >= 0 ? input >> 3 : -((-input) >> 3);
}

static inline s32 divide2(s32 input)
{
    return input >= 0 ? input >> 1 : -((-input) >> 1);
}

static inline s32 SpriteOam_AllocateAffineMatrix(u8 angle, s8 sx, s8 sy, s32 *count)
{
    if (*count > 31)
        return 0;
    gSpriteAffineParams[*count].angle = angle;
    gSpriteAffineParams[*count].scaleX = sx;
    gSpriteAffineParams[*count].scaleY = sy;
    return (*count)++;
}

void SpriteOam_QueueCameraRelativeSprites(void)
{

    struct SpriteDrawable *sprite;
    s32 *matrixCount;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity)
        skip = 0x201;
    sprite = gDrawableListHeads[gRenderListIndex];
    if (!sprite)
        return;
    matrixCount = &gAffineRequestCount;
    do {
        s32 x, y;
        s32 z;
        u32 flags;
        u32 attr;
        s32 depth;
        s32 ox, oy;
        u32 initialFlags = sprite->flags;
        u32 t = initialFlags;
        t &= skip;
        flags = initialFlags;
        if (t) {
            sprite = sprite->next;
            continue;
        }

        x = divide8(sprite->position.x - gCamera.position.x) + 120;
        y = divide8(sprite->position.y - gCamera.position.y) + 80;
        z = sprite->position.z - gCamera.position.z;
        {
            u32 cull = flags & 64;
            ox = sprite->offsetX;
            oy = sprite->offsetY;
            if (!cull) {
                u32 width = sprite->width, height = sprite->height;
                s32 left = x + ox, top = y + oy;
                if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0) {
                    sprite = sprite->next;
                    continue;
                }
            }
        }
        x += ox;
        y += oy;
        depth = z;
        x &= 511;
        {
            u32 shifted = x << 16;
            attr = sprite->attributes | shifted;
        }
        y &= 255;
        attr |= y;
        if (flags & 2) {
            u32 matrix;
            u32 angle;
            s32 sx, sy;
            if (!(flags & 0x20000) && (UnsignedScale(sprite->scaleX) > 60 ||
                                       UnsignedScale(sprite->scaleY) > 60 || sprite->angle != 0))
                attr |= 0x300;
            else
                attr |= 0x100;
            angle = sprite->angle;
            sx = (s8)sprite->scaleX;
            sy = (s8)sprite->scaleY;
            matrix = SpriteOam_AllocateAffineMatrix(angle, sx, sy, &gAffineRequestCount);
            attr |= matrix << 25;
            flags = sprite->flags;
        }
        if (flags & 4)
            attr |= 0x10000000;
        if (flags & 8)
            attr |= 0x20000000;
        if (gSpriteAttrFlags && (flags & 0x4000))
            attr |= 0x400;
        if (((gSpriteMosaicAttr & 1) && sprite->priority && !(flags & 0x10000)) || (flags & 0x800))
            attr |= 0x1000;
        depth += sprite->depthOffset;
        if (flags & 0x20) {
            s32 tile = sprite->tile | (sprite->priority << 10) | (sprite->palette << 12);
            s32 n = gOamCount;
            if (n > 127)
                goto next;
            gOamBuffer[n].attributes = attr;
            gOamBuffer[n].tile = tile;
            gOamCount = n + 1;
            gVideoUploadFlags |= 1;
        } else {
            s32 tile, n;
            u32 priority;
            depth = divide2(depth);
            if (depth < -128)
                depth = -128;
            else if (depth > 127)
                depth = 127;
            tile = sprite->tile;
            priority = sprite->priority;
            tile = tile | (priority << 10) | (sprite->palette << 12);
            n = gPendingOamCount;
            if (n > 127)
                goto next;
            gPendingOamBuffer[n].attributes = attr;
            gPendingOamBuffer[n].tile = tile;
            gPendingOamBuffer[n].depth = ((depth + 128) & 255) << 8;
            gPendingOamCount = n + 1;
            gOamPriorityCounts[priority]++;
        }
    next:
        sprite = sprite->next;
    } while (sprite);
}

// confidence=low: flag 0x1000 selects this sprite staging pass.

void SpriteOam_QueueFlaggedProjectedSprites(void)
{
    struct ContactPosition pos;
    struct SpriteDrawable *sprite;
    s32 *matrixCount;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity)
        skip = 0x201;
    sprite = gDrawableListHeads[gRenderListIndex];
    if (!sprite)
        return;
    matrixCount = &gAffineRequestCount;
    do {
        s32 x, y;
        s32 z;
        u32 flags;
        u32 attr;
        s32 depth;
        s32 ox, oy;
        s32 tmp;
        u32 initialFlags = sprite->flags;
        if (initialFlags & skip) {
            sprite = sprite->next;
            continue;
        }
        if (!(initialFlags & 0x1000))
            goto next;

        if (!(initialFlags & 16)) {
            SpriteOam_ProjectPosition(&pos, &sprite->position);
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = sprite->position.x;
            y = sprite->position.y;
            z = sprite->position.z;
        }
        {
            u32 loaded = sprite->flags;
            u32 cull = loaded & 64;
            flags = loaded;
            ox = sprite->offsetX;
            oy = sprite->offsetY;
            if (!cull) {
                u32 width = sprite->width, height = sprite->height;
                s32 left = x + ox, top = y + oy;
                if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0)
                    goto next;
            }
        }
        x += ox;
        y += oy;
        depth = z;
        x &= 511;
        {
            u32 shifted = x << 16;
            attr = sprite->attributes | shifted;
        }
        y &= 255;
        attr |= y;
        if (flags & 2) {
            u32 matrix;
            u32 angle;
            s32 sx, sy;
            if (!(flags & 0x20000) && (UnsignedScale(sprite->scaleX) > 60 ||
                                       UnsignedScale(sprite->scaleY) > 60 || sprite->angle != 0))
                attr |= 0x300;
            else
                attr |= 0x100;
            angle = sprite->angle;
            sx = (s8)sprite->scaleX;
            sy = (s8)sprite->scaleY;
            if (*matrixCount > 31)
                matrix = 0;
            else {
                s32 *count = matrixCount;
                gSpriteAffineParams[*count].angle = angle;
                gSpriteAffineParams[*count].scaleX = sx;
                gSpriteAffineParams[*count].scaleY = sy;
                tmp = *count;
                matrix = tmp++;
                *count = tmp;
            }
            attr |= matrix << 25;
            flags = sprite->flags;
        }
        if (flags & 4)
            attr |= 0x10000000;
        if (flags & 8)
            attr |= 0x20000000;
        if (gSpriteAttrFlags && (flags & 0x4000))
            attr |= 0x400;
        if (((gSpriteMosaicAttr & 1) && sprite->priority && !(flags & 0x10000)) || (flags & 0x800))
            attr |= 0x1000;
        depth += sprite->depthOffset;
        if (flags & 0x20) {
            s32 tile = sprite->tile | (sprite->priority << 10) | (sprite->palette << 12);
            s32 n = gOamCount;
            if (n > 127)
                goto next;
            gOamBuffer[n].attributes = attr;
            gOamBuffer[n].tile = tile;
            gOamCount = n + 1;
            gVideoUploadFlags |= 1;
        } else {
            s32 tile, n;
            u32 priority;
            tmp = depth >= 0 ? depth >> 1 : -((-depth) >> 1);
            depth = tmp;
            if (depth < -128)
                depth = -128;
            else if (depth > 127)
                depth = 127;
            tile = sprite->tile;
            priority = sprite->priority;
            tile = tile | (priority << 10) | (sprite->palette << 12);
            n = gPendingOamCount;
            if (n > 127)
                goto next;
            gPendingOamBuffer[n].attributes = attr;
            gPendingOamBuffer[n].tile = tile;
            gPendingOamBuffer[n].depth = ((depth + 128) & 255) << 8;
            gPendingOamCount = n + 1;
            gOamPriorityCounts[priority]++;
        }
    next:
        sprite = sprite->next;
    } while (sprite);
}

// confidence=low: flag 0x2000 selects this sprite staging pass.

void SpriteOam_QueueOverlayProjectedSprites(void)
{
    struct ContactPosition pos;
    struct SpriteDrawable *sprite;
    s32 *matrixCount;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity)
        skip = 0x201;
    sprite = gDrawableListHeads[gRenderListIndex];
    if (!sprite)
        return;
    matrixCount = &gAffineRequestCount;
    do {
        s32 x, y;
        s32 z;
        u32 flags;
        u32 attr;
        s32 depth;
        s32 ox, oy;
        s32 tmp;
        u32 initialFlags = sprite->flags;
        if (initialFlags & skip) {
            sprite = sprite->next;
            continue;
        }
        if (!(initialFlags & 0x2000))
            goto next;

        if (!(initialFlags & 16)) {
            SpriteOam_ProjectPosition(&pos, &sprite->position);
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = sprite->position.x;
            y = sprite->position.y;
            z = sprite->position.z;
        }
        {
            u32 loaded = sprite->flags;
            u32 cull = loaded & 64;
            flags = loaded;
            ox = sprite->offsetX;
            oy = sprite->offsetY;
            if (!cull) {
                u32 width = sprite->width, height = sprite->height;
                s32 left = x + ox, top = y + oy;
                if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0)
                    goto next;
            }
        }
        x += ox;
        y += oy;
        depth = z;
        x &= 511;
        {
            u32 shifted = x << 16;
            attr = sprite->attributes | shifted;
        }
        y &= 255;
        attr |= y;
        if (flags & 2) {
            u32 matrix;
            u32 angle;
            s32 sx, sy;
            if (!(flags & 0x20000) && (UnsignedScale(sprite->scaleX) > 60 ||
                                       UnsignedScale(sprite->scaleY) > 60 || sprite->angle != 0))
                attr |= 0x300;
            else
                attr |= 0x100;
            angle = sprite->angle;
            sx = (s8)sprite->scaleX;
            sy = (s8)sprite->scaleY;
            if (*matrixCount > 31)
                matrix = 0;
            else {
                s32 *count = matrixCount;
                gSpriteAffineParams[*count].angle = angle;
                gSpriteAffineParams[*count].scaleX = sx;
                gSpriteAffineParams[*count].scaleY = sy;
                tmp = *count;
                matrix = tmp++;
                *count = tmp;
            }
            attr |= matrix << 25;
            flags = sprite->flags;
        }
        if (flags & 4)
            attr |= 0x10000000;
        if (flags & 8)
            attr |= 0x20000000;
        if (gSpriteAttrFlags && (flags & 0x4000))
            attr |= 0x400;
        if (((gSpriteMosaicAttr & 1) && sprite->priority && !(flags & 0x10000)) || (flags & 0x800))
            attr |= 0x1000;
        depth += sprite->depthOffset;
        if (flags & 0x20) {
            s32 tile = sprite->tile | (sprite->priority << 10) | (sprite->palette << 12);
            s32 n = gOamCount;
            if (n > 127)
                goto next;
            gOamBuffer[n].attributes = attr;
            gOamBuffer[n].tile = tile;
            gOamCount = n + 1;
            gVideoUploadFlags |= 1;
        } else {
            s32 tile, n;
            u32 priority;
            tmp = depth >= 0 ? depth >> 1 : -((-depth) >> 1);
            depth = tmp;
            if (depth < -128)
                depth = -128;
            else if (depth > 127)
                depth = 127;
            tile = sprite->tile;
            priority = sprite->priority;
            tile = tile | (priority << 10) | (sprite->palette << 12);
            n = gPendingOamCount;
            if (n > 127)
                goto next;
            gPendingOamBuffer[n].attributes = attr;
            gPendingOamBuffer[n].tile = tile;
            gPendingOamBuffer[n].depth = ((depth + 128) & 255) << 8;
            gPendingOamCount = n + 1;
            gOamPriorityCounts[priority]++;
        }
    next:
        sprite = sprite->next;
    } while (sprite);
}
