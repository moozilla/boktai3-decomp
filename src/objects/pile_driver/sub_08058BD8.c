#include "engine/video/render.h"
#include "objects/pile_driver/scene_panel_overlay.h"

static inline s32 scaleSceneOverlay(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 5;
    else
        result = -((-value) >> 5);
    return result;
}
static inline void projectSceneOverlay(struct ContactPosition *out,
                                       const struct ContactPosition *origin)
{
    s32 x = origin->x;
    s32 z = origin->z;
    s32 sum, y;
    out->x = scaleSceneOverlay((x - z) * 3);
    sum = scaleSceneOverlay((x + z) * 3);
    y = scaleSceneOverlay(origin->y * 3);
    out->y = sum - y;
    out->z = sum + y;
}
#define truncate5(v)                                                                               \
    ({                                                                                             \
        s32 value = (v);                                                                           \
        s32 result;                                                                                \
        if (value >= 0)                                                                            \
            result = value >> 5;                                                                   \
        else                                                                                       \
            result = -((-value) >> 5);                                                             \
        result;                                                                                    \
    })
#define truncate1(v)                                                                               \
    ({                                                                                             \
        s32 value = (v);                                                                           \
        s32 result;                                                                                \
        if (value >= 0)                                                                            \
            result = value >> 1;                                                                   \
        else                                                                                       \
            result = -((-value) >> 1);                                                             \
        result;                                                                                    \
    })
void ScenePanel_DrawOverlay(struct ScenePanel *panel, s32 tileX, s32 tileY, s32 screenX,
                            s32 screenY)
{
    struct SceneOverlayResource resource;
    struct SceneOverlayMapEntry entry;
    struct ContactPosition projected;
    struct SceneOverlayMapEntry *entries;
    struct BackgroundLayer *bg = &gBgRecords[3];
    u16 *output;
    s32 width, height;
    u16 *blocks;
    s32 blockStride, centerX, centerY;
    s32 playSound = 0;
    s32 slot, x, y, minX, minY, maxX, maxY, target;
    s32 dx, dy;

    ScenePanel_CycleOverlayPalette(panel);
    bg->scaleX = tileX;
    bg->scaleY = tileY;
    bg->x = screenX;
    bg->y = screenY;
    output = bg->map;
    width = bg->width * 2;
    height = bg->height * 2;
    projectSceneOverlay(&projected, &panel->origin);
    centerX = projected.x + gCamera.offsetX;
    centerY = projected.y + gCamera.offsetY;
    centerX = truncate3(centerX);
    centerY = truncate3(centerY) & ~1;
    centerX -= tileX;
    centerY -= tileY;
    resource = *(struct SceneOverlayResource *)panel->overlayMap;
    resource.maps = (void *)((u32)resource.maps + (u32)panel->overlayMap);
    entries = resource.maps;
    panel->overlayChanged = 0;
    for (slot = 0; slot < panel->subjectCount; slot++) {
        if (panel->paletteMode == 0 || (panel->effectState != 10 && panel->effectState != 14))
            target = 5;
        else if (panel->subjects[slot]->state == 4)
            target = 0;
        else
            target = 5;
        panel->targetLevels[slot] = target;
        if (panel->currentLevels[slot] == target)
            continue;
        if (target == 0)
            playSound = 1;
        panel->currentLevels[slot] = target;
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
                maxX = maxY = minY = minX = -1;
                break;
        }
        if (maxX < 0 || maxY < 0 || minX > 31 || minY > 31)
            continue;
        blockStride = bg->width >> 4;
        if (target == 5) {
            for (y = minY; y < maxY; y += 2) {
                for (x = minX; x < maxX; x++) {
                    s32 dx = tileX + x;
                    s32 dy = tileY + y;
                    dx &= 31;
                    dy &= 31;
                    output[dx + (dy << 5)] = 0;
                    output[dx + ((dy + 1) << 5)] = 0;
                }
            }
        } else {
            s32 mapIndex = target + 1;
            entry = entries[mapIndex];
            entry.blocks = (u16 *)((u32)entry.blocks + (u32)panel->overlayMap);
            bg->mapData = entry.blocks;
            blocks = entry.blocks;
            for (y = minY; y < maxY; y += 2) {
                for (x = minX; x < maxX; x++) {
                    dx = tileX + x;
                    dy = tileY + y;
                    if (dx < 0 || dy < 0 || dx >= width || dy >= height) {
                        dx &= 31;
                        dy &= 31;
                        output[dx + (dy << 5)] = 0;
                        output[dx + ((dy + 1) << 5)] = 0;
                    } else {
                        u32 tile;
                        s32 blockX = truncate5(dx);
                        s32 blockY = truncate5(dy);
                        s32 block = (blockX + blockY * blockStride) << 8;
                        s32 halfX, halfY;
                        dx &= 31;
                        dy &= 31;
                        halfX = truncate1(dx);
                        halfY = truncate1(dy);
                        tile = blocks[block + halfX + (halfY << 4)];
                        if ((dx & 1) == 0) {
                            output[dx + (dy << 5)] = bg->extraData[tile * 4];
                            output[dx + ((dy + 1) << 5)] = bg->extraData[tile * 4 + 2];
                        } else {
                            output[dx + (dy << 5)] = bg->extraData[tile * 4 + 1];
                            output[dx + ((dy + 1) << 5)] = bg->extraData[tile * 4 + 3];
                        }
                    }
                }
            }
        }
    }
    if (playSound)
        Sound_PlaySong(245);
}
