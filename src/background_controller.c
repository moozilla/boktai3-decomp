#include "global.h"
#include "render.h"
#include "background_controller.h"
/* Background_LoadLayerMap: Resolves a background resource via sub_08216E54, optionally finds the map id with Background_FindMapId, then calls Bg_SetLayerMap; matches docs' descr */
#define Background_LoadLayerMap sub_080020BC
/* ScrollingBackground_ClearSlots: Zeroes all four ScrollingBackground slots in gScrollingBackgrounds, unlike the single-slot clear done by the destroy callback ScrollingBackground_Destroy */
#define ScrollingBackground_ClearSlots sub_08002284

/* Functions marked MATCHING_CONCESSION keep local register hints made with
 * empty asm constraints; these emit no handwritten instructions. */
s32 Background_ApplyQueuedCommands(struct Background *p)
{
    u32 i;
    for (i = 0; i < p->queue.count; i++) {
        u32 type = p->queue.types[i];
        s16 *a = p->queue.commands[i].args;
        switch (type) {
            case 0:
                p->moveActive = 0;
                p->moveTime = 0;
                break;
            case 1:
                p->moveActive = 1;
                p->moveDuration = a[0];
                p->moveTime = 0;
                p->moveTargetX = a[1] << 7;
                p->moveTargetY = a[2] << 7;
                p->moveOriginX = p->x;
                p->moveOriginY = p->y;
                break;
            case 2:
                p->fadeActive = 0;
                p->blendX = a[0] << 8;
                p->blendY = a[1] << 8;
                p->blendStepX = 0;
                p->blendStepY = 0;
                Blend_SetAlpha(p->layer, p->blendX >> 8, p->blendY >> 8);
                break;
            case 3:
                p->fadeActive = 1;
                p->blendStartX = p->blendX;
                p->blendStartY = p->blendY;
                p->fadeDuration = a[0];
                p->blendTargetX = a[1] << 8;
                p->blendTargetY = a[2] << 8;
                p->blendStepX = Div(p->blendTargetX - p->blendStartX, p->fadeDuration);
                p->blendStepY = Div(p->blendTargetY - p->blendStartY, p->fadeDuration);
                Blend_SetAlpha(p->layer, p->blendX >> 8, p->blendY >> 8);
                break;
            case 6:
                p->driftX = a[0];
                p->driftY = a[1];
                break;
            case 7:
                p->scrollTime = 0;
                p->scrollDuration = a[0];
                p->scrollStartX = p->scrollSpeedX << 7;
                p->scrollStartY = p->scrollSpeedY << 7;
                p->scrollTargetX = a[1] << 7;
                p->scrollTargetY = a[2] << 7;
                if (!p->scrollDuration) {
                    p->scrollSpeedX = a[1];
                    p->scrollSpeedY = a[2];
                }
                break;
        }
    }
    return p->queue.count = 0;
}

// MATCHING_CONCESSION: register-pinning
s32 Background_Update(struct Background *p)
{
    s32 x, y;
    s32 tileX, tileY;

    Background_ApplyQueuedCommands(p);
    tileY = 0;
    if (p->flags & 2) {
        s32 cx, cy;
        register s32 bottom asm("r5");
        if (!gCamera.mode) {
            cx = gUnk_03005418[0] + gCamera.offsetX;
            cy = gUnk_03005418[1] + gCamera.offsetY;
        } else {
            cx = gCamera.position.x >> 3;
            cy = gCamera.position.y >> 3;
        }
        if (!(p->flags & 1)) {
            cx -= p->cameraOffsetX;
            cy -= p->cameraOffsetY;
        }
        if (p->flags & 128) {
            cx += p->driftX;
            cy += p->driftY;
        }
        {
            s32 cyHint = cy;
            cy = cyHint;
        }
        if (p->rateX == 128)
            x = cx;
        else
            x = ({
                s32 delta = cx - p->centerX;
                register s32 result asm("r0") = truncate7(delta * p->rateX) + p->centerX;
                asm("" : "+r"(result));
                result;
            });
        x -= 120;
        if (p->rateY == 128)
            y = cy;
        else
            y = ({
                s32 delta = cy - p->centerY;
                register s32 result asm("r0") = truncate7(delta * p->rateY) + p->centerY;
                asm("" : "+r"(result));
                result;
            });
        y -= 80;
        bottom = y + 80;
        tileX = truncate3(x - 8);
        {
            s32 value = truncate3(bottom - 128);
            s32 mask = -2;
            tileY = mask & value;
        }
    } else {
        if (p->flags & 4) {
            if (p->scrollDuration) {
                s32 remaining = p->scrollDuration - p->scrollTime;
                asm("" : "+r"(remaining));
                p->scrollSpeedX =
                    Div(p->scrollStartX * remaining + p->scrollTargetX * p->scrollTime,
                        p->scrollDuration) /
                    128;
                p->scrollSpeedY =
                    Div(p->scrollStartY * remaining + p->scrollTargetY * p->scrollTime,
                        p->scrollDuration) /
                    128;
                if (++p->scrollTime > p->scrollDuration) {
                    p->scrollDuration = 0;
                    p->scrollTime = 0;
                    p->scrollSpeedX = p->scrollTargetX / 128;
                    p->scrollSpeedY = p->scrollTargetY / 128;
                }
            }
            p->scrollDistX += p->scrollSpeedX;
            p->scrollDistY += p->scrollSpeedY;
            p->x = p->scrollDistX + p->baseX;
            p->y = p->scrollDistY + p->baseY;
        } else if (p->moveActive == 1) {
            if (p->moveDuration) {
                register s32 remaining asm("r5") = p->moveDuration - p->moveTime;
                p->x =
                    Div(p->moveOriginX * remaining + p->moveTargetX * p->moveTime, p->moveDuration);
                p->y =
                    Div(p->moveOriginY * remaining + p->moveTargetY * p->moveTime, p->moveDuration);
            }
            if (++p->moveTime >= p->moveDuration) {
                p->moveActive = 0;
                p->moveTime = 0;
                p->x = p->moveTargetX;
                p->y = p->moveTargetY;
            }
        }
        x = ({
            register s32 value asm("r3") = truncate7(p->x);
            asm("" : "+r"(value));
            value;
        });
        y = ({
            s32 value = truncate7(p->y);
            value;
        });
        if (p->flags & 32) {
            x += gUnk_03005418[0] + gCamera.offsetX;
            y += gUnk_03005418[1] + gCamera.offsetY;
        }
        {
            register s32 rowY asm("r5") = y;
            if (x < 0)
                tileX = ~((-x) >> 3);
            else
                tileX = x >> 3;
            if (rowY < 0) {
                register s32 negative asm("r0") = ~((-rowY) >> 3);
                asm("" : "+r"(negative));
                tileY = negative;
            } else
                tileY = rowY >> 3;
            tileY &= ~1;
        }
    }
    BgGfx_LoadTilemapSimple(p->layer, tileX, tileY, x, y);
    if (p->fadeActive == 1) {
        p->blendX += p->blendStepX;
        p->blendY += p->blendStepY;
        Blend_SetAlpha(p->layer, p->blendX >> 8, p->blendY >> 8);
        if (--p->fadeDuration == 0)
            p->fadeActive = 0;
    }
    if (p->flags & 64) {
        if (p->frame & 1) {
            u32 mask = (0x01000000u << p->layer) >> 16;
            gDispCntShadow = mask | gDispCntShadow;
        } else {
            u32 mask = (0x01000000u << p->layer) >> 16;
            gDispCntShadow &= ~mask;
        }
    }
    p->frame++;
    return 0;
}

u32 Background_Destroy(struct Background *p)
{
    gBackgrounds[p->layer] = 0;
    return 0;
}

static inline u8 modeFlag(struct Background *p)
{
    return p->flags & 16;
}

// MATCHING_CONCESSION: register-pinning
s32 Background_Initialize(struct Background *p, u16 id, struct BackgroundConfig *c)
{
    struct BackgroundLayer *layer;
    u8 *resource;
    s32 offsetX;
    s32 offsetY;
    s32 x;
    s32 y;
    short mode;
    gBackgrounds[c->layer] = p;
    p->id = id;
    p->flags = c->flags;
    p->layer = c->layer;
    layer = &gUnk_03004C30[p->layer];
    p->resource = c->resource;
    resource = sub_08216E54(p->resource);
    if (p->flags & 1) {
        struct BackgroundDimensions *dims = ResourceRegistry_GetResource(0x56C2)->data;
        offsetX = dims->x;
        offsetY = dims->y;
    } else {
        offsetX = 0;
        offsetY = 0;
    }
    if (p->flags & 2) {
        x = gUnk_03005418[0] + gCamera.offsetX;
        y = gUnk_03005418[1] + gCamera.offsetY;
    } else {
        x = c->x;
        y = c->y;
    }
    p->rateX = c->rateX;
    p->rateY = c->rateY;
    layer->x = x - 120;
    layer->y = y - 80;
    p->x = x << 7;
    p->y = y << 7;
    p->baseX = p->x;
    p->baseY = p->y;
    p->moveTargetX = p->x;
    p->moveTargetY = p->y;
    p->scrollSpeedX = c->scrollX;
    p->scrollSpeedY = c->scrollY;
    p->scrollDistX = 0;
    p->scrollDistY = 0;
    p->cameraOffsetX = offsetX >> 7;
    p->cameraOffsetY = offsetY >> 7;
    p->driftX = c->offsetX;
    p->driftY = c->offsetY;
    p->limitX = c->limitX;
    p->limitY = c->limitY;
    p->centerX = c->centerX;
    p->centerY = c->centerY;
    p->moveActive = 0;
    p->moveDuration = 0;
    p->moveTime = 0;
    p->fadeActive = 0;
    p->blendX = c->blendX << 8;
    p->blendY = c->blendY << 8;
    p->blendStepX = 0;
    p->blendStepY = 0;
    p->blendTargetX = p->blendX;
    p->blendTargetY = p->blendY;
    p->blendStepX = p->blendX;
    p->blendStepY = p->blendY;
    mode = modeFlag(p) != 0;
    if (resource[0] == 'M' && resource[1] == 'P') {
        layer->flags = mode;
        Bg_SetPriority(p->layer, c->mode);
    } else {
        void *map = Video_GetBackgroundMap(p->layer);
        u32 layerId = p->layer;
        u32 drawMode = mode;
        Bg_SetupLayer(layerId, drawMode, resource, 0, (short)offsetX, offsetY, c->mode, map);
    }
    if (p->limitX && p->limitY) {
        layer->halfWidth = p->limitX / 2;
        layer->halfHeight = p->limitY / 2;
    }
    return 0;
}

void *Background_Create(u16 id, struct BackgroundConfig *c)
{
    struct Background *p;
    if (c->priority == 0)
        p = Task_Create(9, 0x140);
    else
        p = Task_Create(0xb, 0x140);
    if (p != 0) {
        Task_SetCallbacks(p, Background_Update, Background_Destroy);
        if (Background_Initialize(p, id, c) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

static inline s32 next(s32 fallback)
{
    u8 *p = Script_GetPc();
    if (p)
        return Script_ReadValue(p);
    return fallback;
}

static inline s32 nextZero(void)
{
    s32 v = (s32)Script_GetPc();
    if (v)
        v = Script_ReadValue((u8 *)v);
    return v;
}

void *Background_CreateFromScript(u32 id)
{
    struct BackgroundConfig c;
    c.flags = Script_GetKeywordValue('f', 0);
    c.layer = Script_GetKeywordValue('i', 0);
    c.resource = Script_GetKeywordValue('d', 0);
    if (Script_SeekToKeyword('r')) {
        c.rateX = Script_GetValue();
        c.rateY = Script_GetValue();
    } else {
        c.rateX = 128;
        c.rateY = 128;
    }
    switch (c.layer) {
        case 0:
            c.mode = Script_GetKeywordValue('p', 0);
            break;
        case 2:
            c.mode = Script_GetKeywordValue('p', 3);
            break;
        case 1:
        case 3:
            c.mode = Script_GetKeywordValue('p', 1);
            break;
    }
    c.priority = Script_GetKeywordValue('a', 0);
    if (Script_SeekToKeyword('s')) {
        c.x = nextZero();
        c.y = nextZero();
    } else {
        c.x = 0;
        c.y = 0;
    }
    if (Script_SeekToKeyword('m')) {
        c.scrollX = nextZero();
        c.scrollY = nextZero();
    } else {
        c.scrollX = 0;
        c.scrollY = 0;
    }
    if (Script_SeekToKeyword('l')) {
        c.limitX = nextZero();
        c.limitY = nextZero();
    } else {
        c.limitX = 0;
        c.limitY = 0;
    }
    if (Script_SeekToKeyword('c')) {
        c.centerX = next(120);
        c.centerY = next(80);
    } else {
        c.centerX = 120;
        c.centerY = 80;
    }
    if (Script_SeekToKeyword('A')) {
        c.offsetX = nextZero();
        c.offsetY = nextZero();
    } else {
        c.offsetX = 0;
        c.offsetY = 0;
    }
    if (Script_SeekToKeyword('b')) {
        s32 x = next(0), y = next(8);
        c.blendX = x;
        c.blendY = y;
        Blend_SetAlpha(c.layer, x, y);
    }
    return Background_Create(id, &c);
}

// MATCHING_CONCESSION: register-pinning
s32 Background_FindMapId(struct BackgroundResource *resource, u16 id)
{
    struct BackgroundMapEntry *entry = (struct BackgroundMapEntry *)resource->maps;
    s32 i;
    for (i = 0; i < resource->count; i++) {
        if (entry->id == id) {
            return i;
        }
        do {
            entry++;
        } while (0);
    }
    return 0;
}

static inline s32 nextShift(void)
{
    u8 *p = Script_GetPc();
    return p ? Script_ReadValue(p) << 7 : 0;
}

// MATCHING_CONCESSION: register-pinning
s32 Background_LoadMapsFromScript(void)
{
    s32 indices[4];
    struct BackgroundResource resource;
    u8 *base = sub_08216E54(Script_GetKeywordValue('d', 0));
    s32 target, x, y, n, explicitIndices, i;
    resource = *(struct BackgroundResource *)base;
    resource.maps += (u32)base;
    resource.tiles += (u32)base;
    resource.extra += (u32)base;
    target = Script_GetKeywordValue('t', 1);
    if (Script_SeekToKeyword('o')) {
        u8 *cursor;
        x = ({
            s32 value = nextShift();
            asm("" : "+r"(value));
            value;
        });
        cursor = Script_GetPc();
        if (cursor)
            y = Script_ReadValue(cursor) << 7;
        else
            y = 0;
    } else {
        register s32 zero asm("r2") = 0;
        asm("" : "+r"(zero));
        x = zero;
        y = 0;
    }
    n = Script_GetKeywordValue('n', 1);
    explicitIndices = 0;
    if (Script_SeekToKeyword('i')) {
        u8 *p = Script_GetPc();
        explicitIndices = 1;
        for (i = 0; i < 4; i++) {
            if (p) {
                indices[i] = Script_ReadValue(p);
                p = Script_GetPc();
            } else
                indices[i] = (s32)p;
        }
    } else {
        s32 *out;
        i = 3;
        out = indices + 3;
        for (; i >= 0; i--) {
            *out = i;
            out--;
        }
    }
    if (!explicitIndices && Script_SeekToKeyword('c')) {
        u8 *p = Script_GetPc();
        s32 *out = indices;
        for (i = 3; i >= 0; i--) {
            if (p) {
                *out = Background_FindMapId(&resource, Script_ReadValue(p));
                p = Script_GetPc();
            }
            out++;
        }
    }
    if (base[0] == 'M' && base[1] == 'P') {
        register s32 drawX asm("r5") = x;
        asm("" : "+r"(drawX));
        Bg_SetupLayers(target, 0, base, drawX, y, n, indices);
    }
}

s32 Background_LoadMapFromScript(void)
{
    struct BackgroundResource resource;
    u16 layer = Script_GetKeywordValue('p', 0);
    u16 index;
    u8 *base = sub_08216E54(Script_GetKeywordValue('d', 0));
    resource = *(struct BackgroundResource *)base;
    resource.maps += (u32)base;
    resource.tiles += (u32)base;
    resource.extra += (u32)base;
    if (!({
            u8 found = FALSE;
            u8 *arg;
            if (Script_SeekToKeyword('i') && (arg = Script_GetPc())) {
                index = Script_ReadValue(arg);
                found = TRUE;
            } else
                index = 0;
            found;
        }) &&
        Script_SeekToKeyword('c')) {
        u8 *p = Script_GetPc();
        if (p)
            index = (u16)Background_FindMapId(&resource, Script_ReadValue(p));
    }
    Bg_SetLayerMap(layer, base, index);
    return index;
}

s32 sub_080020BC(s32 layer, u16 resourceId, s32 useIndex, u16 index)
{
    struct BackgroundResource resource;
    u8 *base = sub_08216E54(resourceId);
    resource = *(struct BackgroundResource *)base;
    resource.maps += (u32)base;
    resource.tiles += (u32)base;
    resource.extra += (u32)base;
    if (!useIndex)
        index = Background_FindMapId(&resource, index);
    Bg_SetLayerMap(layer, base, index);
    return index;
}

s32 Background_LoadTilesFromScript(void)
{
    u16 entries[64];
    struct BackgroundResource resource;
    u16 layer = Script_GetKeywordValue('t', 0);
    u16 data = Script_GetKeywordValue('d', 0);
    u16 size = Script_GetKeywordValue('s', 0);
    void *base = ScriptProc_Start(0xCEEF, data);
    s32 count = 0;
    if (Script_SeekToKeyword('c')) {
        s32 i;
        u16 *out;
        i = 0;
        out = entries;
        for (;; i++) {
            u8 *p;
            if (i > 63)
                goto done;
            p = Script_GetPc();
            if (!p)
                goto done;
            *out = Script_ReadValue(p);
            count++;
            out++;
        }
    } else {
        s32 zero = 0;
        u16 *out = entries + 63;
        do {
            *out = zero;
            out--;
        } while ((s32)out >= (s32)entries);
    }
done:
    if (Script_SeekToKeyword('m')) {
        struct BackgroundResource *loaded =
            (struct BackgroundResource *)sub_08216E54(Script_GetValue());
        resource = *loaded;
        size = resource.size;
    }
    sub_08215D40(layer, base, count, entries, size);
    return 0;
}

// MATCHING_CONCESSION: register-pinning
void Background_QueueCommand(u32 layer, u32 type, s32 length, s32 *args)
{
    u32 commandType = type;
    s32 lengthLimit = length;
    s32 *input = args;
    struct Background *p = gBackgrounds[layer];
    if (p && p->queue.count <= 7) {
        s32 j;
        p->queue.types[p->queue.count] = commandType;
        j = 0;
        if (j < lengthLimit) {
            /* Retain the original count reload on each argument copy. The byte offset
             * is commands[count].args[j]; this operand order keeps agbcc's index order. */
            volatile u32 *count = &p->queue.count;
            struct BackgroundCommand *commands = p->queue.commands;
            s32 *source = input;
            do {
                *(s16 *)((u8 *)commands + ((j << 1) + (*count << 4))) = *source++;
                j++;
            } while (j < length);
        }
        p->queue.count++;
    }
}

void Background_QueueCommandFromScript(void)
{
    s32 buf[8];
    u32 a = Script_GetKeywordValue(0x74, 1);
    u32 b = Script_GetKeywordValue(0x72, 0);
    s32 n = 0;
    if (Script_SeekToKeyword(0x70)) {
        s32 *q = buf;
        while (Script_GetPc() != 0 && n <= 7) {
            *q++ = Script_GetValue();
            n++;
        }
    }
    Background_QueueCommand(a, b, n, buf);
}

/* Clears the four ScrollingBackground slots in gScrollingBackgrounds. Not listed in
 * symbols/functions.csv; it sits between two units as raw bytes in gen/code.s,
 * so it is decompiled here to keep the subsystem contiguous. */

void sub_08002284(void)
{
    u32 *base = (u32 *)gScrollingBackgrounds;
    u32 zero = 0;
    u32 *p = base + 3;
    do {
        *p-- = zero;
    } while ((s32)p >= (s32)base);
}

static inline s32 fixed4(s32 v)
{
    return v << 4;
}

s32 ScrollingBackground_ApplyCommands(struct ScrollingBackground *p)
{
    u32 i;
    for (i = 0; i < p->queue.count; i++) {
        u32 type = p->queue.types[i];
        s16 *a = p->queue.commands[i].args;
        switch (type) {
            case 0:
                p->moveActive = 0;
                p->moveTime = 0;
                p->moveDx = 0;
                p->moveDy = 0;
                break;
            case 1:
                p->moveActive = 1;
                p->moveDuration = a[0];
                p->moveTime = 0;
                p->moveTargetX = fixed4(a[1]);
                p->moveTargetY = fixed4(a[2]);
                p->moveDx = Div(p->moveTargetX - p->x, p->moveDuration);
                p->moveDy = Div(p->moveTargetY - p->y, p->moveDuration);
                break;
            case 2:
                Blend_SetAlpha(p->layer, a[0], a[1]);
                break;
            case 3:
                p->fadeActive = 1;
                p->blendStartX = p->blendX;
                p->blendStartY = p->blendY;
                p->fadeDuration = a[0];
                p->blendTargetX = a[1] << 8;
                p->blendTargetY = a[2] << 8;
                p->blendStepX = Div(p->blendTargetX - p->blendStartX, p->fadeDuration);
                p->blendStepY = Div(p->blendTargetY - p->blendStartY, p->fadeDuration);
                Blend_SetAlpha(p->layer, p->blendX >> 8, p->blendY >> 8);
                break;
            case 4:
                p->enabled = 0;
                break;
            case 5:
                p->enabled = 1;
                break;
        }
    }
    return 0;
}

static inline s32 pixel(s16 v)
{
    if (v >= 0)
        return v >> 4;
    return -((-v) >> 4);
}

s32 ScrollingBackground_Update(struct ScrollingBackground *p)
{
    struct BackgroundLayer *layer = &gUnk_03004C30[p->layer];
    s32 width, height;
    s32 cx, cy;
    ScrollingBackground_ApplyCommands(p);
    if (!p->enabled)
        return 0;
    width = p->width << 4;
    height = p->height << 4;
    if (p->flags & 8) {
        s32 speed = gUnk_02000550 * (s16)p->speedScale;
        s32 angle = (gUnk_0200053C + 160) & 255;
        s32 x, y;
        const s16 *sine = gUnk_086149C4;
        p->accumulatedX += sine[(angle + 64) & 255] * speed;
        p->accumulatedY += sine[angle & 255] * speed;
        x = fixedToInt(p->accumulatedX);
        y = fixedToInt(p->accumulatedY);
        if (x > width)
            x -= width;
        else if (x < -width)
            x += width;
        if (y > height)
            y -= height;
        else if (y < -height)
            y += height;
        p->x = p->baseX + x;
        p->y = p->baseY + y;
        layer->x = p->x;
        layer->y = p->y;
    } else {
        p->accumulatedX += p->speedX;
        if (p->accumulatedX > width)
            p->accumulatedX -= width;
        else if (p->accumulatedX < -width)
            p->accumulatedX += width;
        p->accumulatedY += p->speedY;
        if (p->accumulatedY > height)
            p->accumulatedY -= height;
        else if (p->accumulatedY < -height)
            p->accumulatedY += height;
        p->x = p->accumulatedX + p->baseX;
        p->y = p->accumulatedY + p->baseY;
        layer->x = pixel(p->x);
        layer->y = pixel(p->y);
    }
    cx = gUnk_03005418[0] + gCamera.offsetX;
    cy = gUnk_03005418[1] + gCamera.offsetY;
    if (p->width)
        layer->x += Mod(cx - 120, p->width);
    else {
        s32 shifted = (u16)layer->x - 120;
        layer->x = shifted + cx;
    }
    if (p->height)
        layer->y += Mod(cy - 80, p->height);
    else {
        s32 shifted = (u16)layer->y - 80;
        layer->y = shifted + cy;
    }
    if (p->fadeActive == 1) {
        p->blendX += p->blendStepX;
        p->blendY += p->blendStepY;
        Blend_SetAlpha(p->layer, p->blendX >> 8, p->blendY >> 8);
        if (--p->fadeDuration == 0)
            p->fadeActive = 0;
    }
}

u32 ScrollingBackground_Destroy(struct ScrollingBackground *p)
{
    /* The slot table is pointer-sized; a u32 view keeps the original access shape. */
    u32 *q = (u32 *)gScrollingBackgrounds;
    u32 i = p->layer;
    q[i] = 0;
}

s32 ScrollingBackground_Initialize(struct ScrollingBackground *p, u16 id,
                                   struct ScrollingBackgroundConfig *c)
{
    struct BackgroundLayer *layer;
    u8 *resource;
    s32 x, y, scale, tx, ty;
    void *map;
    gScrollingBackgrounds[c->layer] = p;
    p->id = id;
    p->flags = c->flags;
    p->layer = c->layer;
    layer = &gUnk_03004C30[p->layer];
    p->resource = c->resource;
    resource = sub_08216E54(p->resource);
    if (p->flags & 1) {
        struct BackgroundDimensions *dims = ResourceRegistry_GetResource(0x56C2)->data;
        c->offsetX = dims->x;
        c->offsetY = dims->y;
    }
    if (p->flags & 2) {
        x = gUnk_03005418[0] + gCamera.offsetX;
        y = gUnk_03005418[1] + gCamera.offsetY;
        scale = c->scale;
    } else {
        x = c->x;
        y = c->y;
        scale = c->scale;
    }
    p->x = x << 4;
    p->y = y << 4;
    p->baseX = p->x;
    p->baseY = p->y;
    p->moveTargetX = p->x;
    p->moveTargetY = p->y;
    p->speedX = c->speedX;
    p->speedY = c->speedY;
    p->accumulatedX = 0;
    p->accumulatedY = 0;
    p->offsetX = c->offsetX;
    p->offsetY = c->offsetY;
    p->width = c->width << 3;
    p->height = c->height << 3;
    p->moveActive = 0;
    p->moveDuration = 0;
    p->moveTime = 0;
    p->enabled = 1;
    map = Video_GetBackgroundMap(p->layer);
    if (resource[0] == 'M' && resource[1] == 'P') {
        layer->flags = 1;
        Bg_SetPriority(p->layer, c->mode);
    } else
        Bg_SetupLayer(p->layer, 1, resource, 0, (s16)c->offsetX, (s16)c->offsetY, c->mode, map);
    p->speedScale = scale;
    {
        s32 drawX = p->x >> 4, drawY = p->y >> 4;
        tx = Div(drawX - 128, 8);
        ty = Div(drawY - 128, 8) & ~1;
        BgGfx_LoadTilemapSimple(p->layer, tx, ty, drawX, drawY);
    }
    p->fadeActive = 0;
    {
        s32 blendX = c->blendX, blendY;
        p->blendX = blendX << 8;
        blendY = c->blendY;
        p->blendY = blendY << 8;
        p->blendStepX = 0;
        p->blendStepY = 0;
        p->blendTargetX = p->blendX;
        p->blendTargetY = p->blendY;
        p->blendStepX = p->blendX;
        p->blendStepY = p->blendY;
        if (c->fade)
            Blend_SetAlpha(p->layer, blendX, blendY);
    }
    return 0;
}

void *ScrollingBackground_Create(u16 id, struct ScrollingBackgroundConfig *c)
{
    struct ScrollingBackground *p;
    if (c->priority == 0)
        p = Task_Create(9, 0xf8);
    else
        p = Task_Create(0xb, 0xf8);
    if (p != 0) {
        Task_SetCallbacks(p, ScrollingBackground_Update, ScrollingBackground_Destroy);
        if (ScrollingBackground_Initialize(p, id, c) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

static inline s32 nextOffset(void)
{
    s32 v = (s32)Script_GetPc();
    if (v)
        v = Script_ReadValue((u8 *)v) << 4;
    return v;
}

void *ScrollingBackground_CreateFromScript(u32 id)
{
    struct ScrollingBackgroundConfig c;
    c.flags = Script_GetKeywordValue('f', 0);
    c.layer = Script_GetKeywordValue('i', 0);
    c.resource = Script_GetKeywordValue('d', 0);
    c.scale = Script_GetKeywordValue('r', 128);
    c.priority = Script_GetKeywordValue('a', 0);
    if (Script_SeekToKeyword('s')) {
        c.x = nextZero();
        c.y = nextZero();
    } else {
        c.x = 0;
        c.y = 0;
    }
    if (Script_SeekToKeyword('o')) {
        c.offsetX = nextOffset();
        c.offsetY = nextOffset();
    } else {
        c.offsetX = 0;
        c.offsetY = 0;
    }
    if (Script_SeekToKeyword('m')) {
        c.speedX = nextZero();
        c.speedY = nextZero();
    } else {
        c.speedX = 0;
        c.speedY = 0;
    }
    if (Script_SeekToKeyword('l')) {
        c.width = nextZero();
        c.height = nextZero();
    } else {
        c.width = 0;
        c.height = 0;
    }
    if (Script_SeekToKeyword('b')) {
        s32 x = next(0), y = next(8);
        c.blendX = x;
        c.blendY = y;
        c.fade = 1;
    } else {
        c.blendX = 16;
        c.blendY = 16;
        c.fade = 0;
    }
    switch (c.layer) {
        case 0:
            c.mode = Script_GetKeywordValue('p', 0);
            break;
        case 2:
            c.mode = Script_GetKeywordValue('p', 3);
            break;
        case 1:
        case 3:
            c.mode = Script_GetKeywordValue('p', 1);
            break;
    }
    return ScrollingBackground_Create(id, &c);
}

void ScrollingBackground_QueueCommand(u32 layer, u32 type, s32 length, s32 *args)
{
    struct ScrollingBackground *p = gScrollingBackgrounds[layer];
    if (p && p->queue.count <= 7) {
        s32 j;
        p->queue.types[p->queue.count] = type;
        for (j = 0; j < length; j++)
            p->queue.commands[p->queue.count].args[j] = *args++;
        p->queue.count++;
    }
}

void ScrollingBackground_QueueCommandFromScript(void)
{
    s32 buf[8];
    u32 a = Script_GetKeywordValue(0x74, 1);
    u32 b = Script_GetKeywordValue(0x72, 0);
    s32 n = 0;
    if (Script_SeekToKeyword(0x70)) {
        s32 *q = buf;
        while (Script_GetPc() != 0 && n <= 7) {
            *q++ = Script_GetValue();
            n++;
        }
    }
    ScrollingBackground_QueueCommand(a, b, n, buf);
}
