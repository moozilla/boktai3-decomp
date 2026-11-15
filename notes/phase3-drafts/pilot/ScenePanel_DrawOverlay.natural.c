#include "render.h"
#include "scene_panel_overlay.h"

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
#define truncate5(value) ((value) >= 0 ? (value) >> 5 : -((-(value)) >> 5))
#define truncate1(value) ((value) >= 0 ? (value) >> 1 : -((-(value)) >> 1))
void ScenePanel_DrawOverlay(struct ScenePanel *panel, s32 tileX, s32 tileY, s32 screenX,
                            s32 screenY)
{
    struct SceneOverlayResource resource;
    struct SceneOverlayMapEntry entry;
    struct ContactPosition projected;
    struct SceneOverlayMapEntry *entries;
    u16 *output, *blocks;
    s32 width, height, blockStride, centerX, centerY;
    s32 playSound = 0;
    s32 slot, x, y, minX, minY, maxX, maxY, target;

    ScenePanel_CycleOverlayPalette(panel);
    gBgRecords[3].scaleX = tileX;
    gBgRecords[3].scaleY = tileY;
    gBgRecords[3].x = screenX;
    gBgRecords[3].y = screenY;
    output = gBgRecords[3].map;
    width = gBgRecords[3].width * 2;
    height = gBgRecords[3].height * 2;
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
        target = 5;
        if (panel->paletteMode != 0 && (panel->effectState == 10 || panel->effectState == 14)) {
            target = 5;
            if (panel->subjects[slot]->state == 4)
                target = 0;
        }
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
                minX = minY = maxY = maxX = -1;
                break;
        }
        if (maxX < 0 || maxY < 0 || minX > 31 || minY > 31)
            continue;
        blockStride = gBgRecords[3].width >> 4;
        if (target == 5) {
            for (y = minY; y < maxY; y += 2) {
                for (x = minX; x < maxX; x++) {
                    s32 dx = (tileX + x) & 31;
                    s32 dy = (tileY + y) & 31;
                    output[dy * 32 + dx] = 0;
                    output[(dy + 1) * 32 + dx] = 0;
                }
            }
        } else {
            entry = entries[target + 1];
            entry.blocks = (u16 *)((u32)entry.blocks + (u32)panel->overlayMap);
            gBgRecords[3].mapData = entry.blocks;
            blocks = entry.blocks;
            for (y = minY; y < maxY; y += 2) {
                for (x = minX; x < maxX; x++) {
                    s32 dx = tileX + x;
                    s32 dy = tileY + y;
                    if (dx < 0 || dy < 0 || dx >= width || dy >= height) {
                        dx &= 31;
                        dy &= 31;
                        output[dy * 32 + dx] = 0;
                        output[(dy + 1) * 32 + dx] = 0;
                    } else {
                        u32 tile;
                        s32 block = (truncate5(dx) + truncate5(dy) * blockStride) << 8;
                        dx &= 31;
                        dy &= 31;
                        tile = blocks[block + truncate1(dx) + (truncate1(dy) << 4)];
                        if ((dx & 1) == 0) {
                            output[dy * 32 + dx] = gBgRecords[3].extraData[tile * 4];
                            output[(dy + 1) * 32 + dx] = gBgRecords[3].extraData[tile * 4 + 2];
                        } else {
                            output[dy * 32 + dx] = gBgRecords[3].extraData[tile * 4 + 1];
                            output[(dy + 1) * 32 + dx] = gBgRecords[3].extraData[tile * 4 + 3];
                        }
                    }
                }
            }
        }
    }
    if (playSound)
        Sound_PlaySong(245);
}
