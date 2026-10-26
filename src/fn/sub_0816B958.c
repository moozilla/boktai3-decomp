// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"

#define MapMenu_GetMarkerKind    MapScreen_GetMarkerType
#define MapMenu_GetMarkerVariant MapScreen_GetMarkerFlipFlags
#define MapMenu_GetTileAddress   Hud_GetTilemapCell
struct MapMenuMarker {
    u8 type, page, x, y;
};
struct MapMenu {
    u8 unknown0[0x4900];
    struct MapMenuMarker *markers;
    u8 page;
    u8 unknown4905[0x23];
    u32 hideSpecialMarkers;
};
u32 MapScreen_GetMarkerType(u32);
s32 MapScreen_GetMarkerFlipFlags(s32);
u16 *Hud_GetTilemapCell(u32, s32, s32);
static inline s32 MapMenu_MarkerAttributes(s32 variant)
{
    if (variant) {
        register s32 attributes asm("r2") = (s16)0xE400;
        asm("" : "+r"(attributes));
        return attributes;
    } else {
        register s32 attributes asm("r1") = (s16)0xE000;
        asm("" : "+r"(attributes));
        return attributes;
    }
}
static inline struct MapMenuMarker *MarkerAt(struct MapMenuMarker **array, s32 index)
{
    register s32 offset asm("r1") = index * 4;
    register struct MapMenuMarker *base asm("r0") = *array;
    return (struct MapMenuMarker *)((u8 *)base + offset);
}
void MapMenu_DrawActiveMarkers(struct MapMenu *menu, s32 left, s32 top)
{
    s32 i;
    register struct MapMenuMarker *marker asm("r6");
    u32 kind;
    u32 tile;
    u16 *dest;
    s32 x, y;
    if (menu->markers == 0)
        return;
    for (i = 0; i <= 31; i++) {
        marker = MarkerAt(&menu->markers, i);
        kind = MapScreen_GetMarkerType(marker->type);
        if (kind == 0)
            break;
        if (marker->page != menu->page)
            continue;
        if (!((*(s32 *)((u8 *)gGameState + (s16)gGameState->savedX * 4 + 0x5C0) >> i) & 1))
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
        {
            register s32 attributes asm("r0") =
                MapMenu_MarkerAttributes(MapScreen_GetMarkerFlipFlags(marker->type));
            asm("" : "+r"(attributes));
            *dest = tile | attributes;
        }
    }
}
