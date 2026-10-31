#include "render.h"
#include "scene_panel_overlay.h"

// MATCHING_CONCESSION: register-pinning

static inline s32 scaleSceneOverlay(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
#define truncate5(value) ((value) >= 0 ? (value) >> 5 : -((-(value)) >> 5))
#define truncate1(value) ((value) >= 0 ? (value) >> 1 : -((-(value)) >> 1))
void ScenePanel_DrawOverlay(struct ScenePanel *panel, s32 tileX, s32 tileY, s32 screenX,
                            s32 screenY)
{
    struct SceneOverlayResource resource;
    struct SceneOverlayMapEntry entry;
    struct ContactPosition projected;
    struct ContactPosition *projectedPointer;
    struct ContactPosition *projectedRead;
    struct SceneOverlayMapEntry *entries;
    s32 width, height;
    u16 *blocks;
    s32 blockStride, centerX, centerY, playSound = 0;
    struct SceneOverlayMapEntry *entryPointer;
    s32 next;
    register u16 *output asm("r9");
    s32 slot;
    ScenePanel_CycleOverlayPalette(panel);
    {
        struct BackgroundLayer *layer;
        register u16 tx asm("r2");
        u16 ty;
        u16 *buffer;
        register u32 reloadScratch asm("r3");
        tx = tileX;
        layer = &gBgRecords[3];
        layer->scaleX = tx;
        ty = tileY;
        layer->scaleY = ty;
        layer->x = screenX;
        layer->y = screenY;
        buffer = layer->map;
        asm("" : : "r"(buffer));
        output = buffer;
        asm("" : : "r"(output));
        /* Defined opaque scratch keeps the signed-load reloads in their original registers. */
        asm("" : "=r"(reloadScratch));
        width = layer->width * 2;
        asm("" : : "r"(width), "r"(tx), "r"(reloadScratch), "r"(buffer));
        height = layer->height * 2;
    }
    {
        s32 x, z, sum;
        s32 y;
        const struct ContactPosition *origin;
        projectedPointer = &projected;
        asm("" : "+r"(projectedPointer));
        origin = &panel->origin;
        asm("" : "+r"(origin));
        x = origin->x;
        z = origin->z;
        {
            s32 value = 3 * (x - z);
            projectedRead = projectedPointer;
            projectedPointer->x = scaleSceneOverlay(value);
        }
        {
            s32 subtotal = x + z;
            s32 value;
            value = scaleSceneOverlay(3 * subtotal);
            sum = value;
        }
        y = scaleSceneOverlay(3 * origin->y);
        projectedPointer->y = sum - y;
        {
            s32 projectedZ = sum + y;
            projectedPointer->z = projectedZ;
        }
    }
    centerX = projectedRead->x + gCamera.offsetX;
    centerY = projected.y + gCamera.offsetY;
    centerX = truncate3(centerX);
    centerY = truncate3(centerY) & ~1;
    centerX -= tileX;
    centerY -= tileY;
    {
        struct SceneOverlayResource *mapBase = panel->overlayMap;
        resource = *mapBase;
        resource.maps = (void *)((u32)resource.maps + (u32)mapBase);
        entries = resource.maps;
    }
    panel->overlayChanged = 0;
    slot = 0;
    if (slot < panel->subjectCount) {
        entryPointer = &entry;
        do {
            s32 old;
            s32 target;
            register u8 *current asm("r2");
            register u8 *base asm("r0");
            register s32 maxX asm("r12"), maxY;
            s32 minX;
            register s32 minY asm("r2");
            if (panel->paletteMode == 0 || (panel->effectState != 10 && panel->effectState != 14))
                target = 5;
            else {
                u32 state = panel->subjects[slot]->state;
                target = 5;
                if (state == 4)
                    target = 0;
            }
            panel->targetLevels[slot] = target;
            base = panel->currentLevels;
            current = base + slot;
            old = *current;
            next = slot + 1;
            if (old == target)
                goto nextSlot;
            if (target == 0)
                playSound = 1;
            *current = target;
            switch (slot) {
                case 0:
                    maxX = centerX;
                    maxY = centerY;
                    minX = maxX - 7;
                    if (minX < 0)
                        minX = 0;
                    minY = maxY - 6;
                    if (minY < 0)
                        minY = 0;
                    break;
                case 2:
                    maxX = centerX + 7;
                    maxY = centerY;
                    minX = centerX;
                    if (minX < 0)
                        minX = 0;
                    minY = maxY - 6;
                    if (minY < 0)
                        minY = 0;
                    break;
                case 3:
                    maxX = centerX;
                    maxY = centerY + 4;
                    minX = maxX - 7;
                    if (minX < 0)
                        minX = 0;
                    minY = centerY;
                    if (minY < 0)
                        minY = 0;
                    break;
                case 1:
                    maxX = centerX + 7;
                    maxY = centerY + 4;
                    minX = centerX;
                    if (minX < 0)
                        minX = 0;
                    minY = centerY;
                    if (minY < 0)
                        minY = 0;
                    break;
                default:
                    minX = -1;
                    minY = minX;
                    maxY = minX;
                    maxX = maxY;
                    break;
            }
            asm("" : "+r"(maxX), "+r"(maxY), "+r"(minY), "+r"(slot) : "r"(target));
            slot++;
            next = slot;
            if (maxX < 0 || maxY < 0 || minX > 31 || minY > 31)
                goto nextSlot;
            {
                struct BackgroundLayer *strideLayer = &gBgRecords[3];
                blockStride = strideLayer->width >> 4;
            }
            if (target == 5) {
                register s32 y asm("r6");
                for (y = minY; y < maxY; y += 2) {
                    register s32 x asm("r5");
                    for (x = minX; x < maxX; x++) {
                        register s32 dx = tileX + x, dy = tileY + y;
                        dx &= 31;
                        dy &= 31;
                        {
                            register s32 index asm("r0") = dy * 32;
                            asm("" : "+r"(index));
                            index = dx + index;
                            output[index] = 0;
                        }
                        dy++;
                        dy *= 32;
                        dx += dy;
                        output[dx] = 0;
                    }
                }
            } else {
                register s32 y asm("r6");
                u32 ringMask;
                target++;
                {
                    u32 offset = target * sizeof(entry);
                    struct SceneOverlayMapEntry *destination, *mapEntries;
                    destination = entryPointer;
                    mapEntries = entries;
                    offset += (u32)mapEntries;
                    *destination = *(struct SceneOverlayMapEntry *)offset;
                }
                {
                    void **mapSlot;
                    struct ScenePanel *mapPanel = panel;
                    u32 mapOffset = 0x4DC;
                    struct SceneOverlayMapEntry *copied;
                    u32 relative;
                    mapSlot = (void *)((u8 *)mapPanel + mapOffset);
                    copied = entryPointer;
                    relative = (u32)copied->blocks;
                    {
                        u32 mapBase = (u32)*mapSlot;
                        relative += mapBase;
                        copied->blocks = (void *)relative;
                        gBgRecords[3].mapData = (void *)relative;
                        blocks = (void *)relative;
                    }
                }
                for (y = minY; y < maxY; y += 2) {
                    s32 x;
                    for (x = minX; x < maxX; x++) {
                        s32 dx, dy;
                        u16 *secondOutput;
                        u32 secondValue;
                        ringMask = 31;
                        dx = tileX + x;
                        dy = tileY + y;
                        if (dx < 0 || dy < 0 || dx >= width || dy >= height) {
                            {
                                u32 mask = ringMask;
                                dx &= mask;
                                dy &= mask;
                            }
                            {
                                register s32 index asm("r0") = dy * 32;
                                asm("" : "+r"(index));
                                index = dx + index;
                                index = (u32)(output + index);
                                secondValue = 0;
                                *(u16 *)index = secondValue;
                            }
                            {
                                register s32 index asm("r0") = dy + 1;
                                index *= 32;
                                asm("" : "+r"(index));
                                index = dx + index;
                                secondOutput = output + index;
                            }
                        } else {
                            register s32 blockX, block, halfX, halfY asm("r0");
                            u32 tile;
                            {
                                s32 scaled = truncate5(dx);
                                blockX = scaled;
                            }
                            {
                                register s32 scaled asm("r0") = truncate5(dy);
                                scaled *= blockStride;
                                scaled = blockX + scaled;
                                block = scaled << 8;
                            }
                            dx &= ringMask;
                            dy &= ringMask;
                            {
                                s32 scaled = truncate1(dx);
                                halfX = scaled;
                            }
                            halfY = truncate1(dy);
                            halfY <<= 4;
                            {
                                s32 index = block + halfX + halfY;
                                tile = blocks[index];
                            }
                            {
                                u32 parity = dx & 1;
                                if (parity == 0) {
                                    u16 *firstOutput;
                                    u16 *tileBuffer, *pair;
                                    {
                                        register s32 index asm("r1") = dy * 32;
                                        index = dx + index;
                                        firstOutput = output + index;
                                    }
                                    /* Preserve the original destination spill while loading the tile table. */
                                    asm(""
                                        :
                                        : "m"(((struct BackgroundLayer *)SCENE_OVERLAY_LAYER_ADDR)
                                                  ->extraData),
                                          "r"(parity));
                                    tileBuffer =
                                        ((struct BackgroundLayer *)SCENE_OVERLAY_LAYER_ADDR)
                                            ->extraData;
                                    tile <<= 3;
                                    tile += (u32)tileBuffer;
                                    pair = (void *)tile;
                                    {
                                        u32 value = *pair;
                                        *firstOutput = value;
                                    }
                                    {
                                        register s32 index asm("r0") = dy + 1;
                                        index *= 32;
                                        index = dx + index;
                                        secondOutput = output + index;
                                    }
                                    secondValue = pair[2];
                                } else {
                                    u16 *firstOutput;
                                    u16 *tileBuffer, *pair;
                                    {
                                        register s32 index asm("r1") = dy * 32;
                                        index = dx + index;
                                        firstOutput = output + index;
                                    }
                                    tileBuffer =
                                        ((struct BackgroundLayer *)SCENE_OVERLAY_LAYER_ADDR)
                                            ->extraData;
                                    tile <<= 3;
                                    tile += (u32)tileBuffer;
                                    pair = (void *)tile;
                                    {
                                        u32 value = pair[1];
                                        *firstOutput = value;
                                    }
                                    {
                                        register s32 index asm("r0") = dy + 1;
                                        index *= 32;
                                        index = dx + index;
                                        secondOutput = output + index;
                                    }
                                    secondValue = pair[3];
                                }
                            }
                        }
                        *secondOutput = secondValue;
                    }
                }
            }
        nextSlot:
            slot = next;
        } while (slot < panel->subjectCount);
    }
    if (playSound)
        Sound_PlaySong(245);
}
