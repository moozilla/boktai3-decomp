#include "global.h"
#include "system/game_state/game_state.h"
#include "ui/pause/map_menu.h"

struct MapMenuMarker {
    u8 type, page, x, y;
};
u32 MapScreen_GetMarkerType(u32);
s32 MapScreen_GetMarkerFlipFlags(s32);
u16 *Hud_GetTilemapCell(u32, s32, s32);
static inline s16 MapMenu_MarkerAttributes(s32 flip, s32 normal, s32 flipped)
{
    if (flip)
        return flipped;
    return normal;
}
void MapMenu_DrawActiveMarkers(struct MapMenu *menu, s32 left, s32 top)
{
    s32 normal = (s16)0xE000;
    s32 flipped = (s16)0xE400;
    s32 i;
    struct MapMenuMarker *marker;
    u32 kind;
    u32 tile;
    u16 *dest;
    s32 x, y;
    if (menu->markers == 0)
        return;
    for (i = 0; i <= 31; i++) {
        marker = &menu->markers[i];
        kind = MapScreen_GetMarkerType(marker->type);
        if (kind == 0)
            break;
        if (marker->page != menu->index)
            continue;
        if (!((((s32 *)gGameState->unk5C0)[(s16)gGameState->savedX] >> i) & 1))
            continue;
        x = marker->x;
        y = marker->y;
        if (x < left || x >= left + 32 || y < top || y >= top + 32)
            continue;
        tile = 0;
        switch (kind) {
            case 1:
                tile = 15;
                break;
            case 3:
            case 4:
                tile = 1;
                break;
            case 5:
                if (!menu->hideSpecialMarkers)
                    tile = 21;
                break;
            case 6:
                if (!menu->hideSpecialMarkers)
                    tile = 19;
                break;
        }
        dest = Hud_GetTilemapCell(1, x, y);
        *dest = tile | MapMenu_MarkerAttributes(MapScreen_GetMarkerFlipFlags(marker->type), normal,
                                                flipped);
    }
}
