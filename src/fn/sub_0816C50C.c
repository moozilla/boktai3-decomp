/* Provisional names and object interpretation; offsets follow static accesses. */
#include "global.h"
#include "game_state.h"
struct MapMarkerLocation {
    u8 map, x, y, pad;
};
struct MapMenu {
    u8 pad[0x48fc];
    struct MapMarkerLocation *locations;
    u8 pad4900[0x1c];
    u16 map, x, y, otherMap, otherX, otherY;
};
void MapMenu_PositionMarkers(struct MapMenu *s)
{
    struct GameState *g = gGameState;
    s->map = s->locations[(s16)g->savedY].map;
    s->x = s->locations[(s16)g->savedY].x * 4;
    s->y = s->locations[(s16)g->savedY].y * 4;
    if ((s16)g->savedX == (s16)g->savedZ && g->unk5AA >= 0) {
        s->otherMap = s->locations[(s16)g->unk5B2].map;
        s->otherX = s->locations[(s16)g->unk5B2].x * 4;
        s->otherY = s->locations[(s16)g->unk5B2].y * 4;
        if ((s16)g->savedY == (s16)g->unk5B2) {
            s->x -= 4;
            s->otherX += 4;
        }
    } else
        s->otherMap = 255;
}
