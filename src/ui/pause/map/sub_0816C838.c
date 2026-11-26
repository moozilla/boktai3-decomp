/* Provisional names and object interpretation; offsets follow static accesses. */
#define MapMenu_IsAreaUnlocked GameState_GetMapRevealFlag
#include "global.h"
struct MapMenuEntry {
    u8 area, map;
    u8 pad[14];
};
struct MapMenu {
    u8 pad0[0x1c];
    void *layerSource, *layer;
    u8 pad24[0x30];
    void *panel;
    u8 pad58[0x48f8 - 0x58];
    struct MapMenuEntry *entries;
    u8 pad48fc[8];
    u8 index, unlocked;
    u8 pad4906[6];
    u16 scrollX, scrollY;
    u8 pad4910[8];
    u16 originX, originY;
    u16 map, x, y;
};
void MapMenu_PositionMarkers(struct MapMenu *);
u32 GameState_GetMapRevealFlag(u32);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void MapMenu_InitializePageBounds(struct MapMenu *);
void MapMenu_ClampScrollAndUpdateArrows(struct MapMenu *);
void MapMenu_RefreshMarkers(struct MapMenu *);
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void MapMenu_DrawActiveMarkers(struct MapMenu *, s32, s32);
void MapScreen_ClearMapLayer(struct MapMenu *);
void MapMenu_ResetMarkers(struct MapMenu *);
s32 MessageWindow_SetMessageTable(const void *);
void MessageWindow_SetPalette(int);
s32 MessageWindow_SetDisplayMode(s32);
void SceneSequence_SelectEntryMessage(struct MapMenu *);
void MapMenu_UpdateInput(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MapMenu_InitializeDisplay(struct MapMenu *s)
{
    u32 zero;
    s->layer = s->layerSource;
    MapMenu_PositionMarkers(s);
    s->index = s->map;
    if (s->entries && GameState_GetMapRevealFlag(s->entries[s->index].area))
        s->unlocked = 1;
    else
        s->unlocked = 0;
    zero = 0;
    Bg_SetupLayers(6, 2, s->layer, 0, 0, 1, &zero);
    if (s->unlocked) {
        s32 dx, dy, tx, ty;
        MapMenu_InitializePageBounds(s);
        s->scrollX = s->x + s->originX - 120;
        s->scrollY = s->y + s->originY - 80;
        MapMenu_ClampScrollAndUpdateArrows(s);
        MapMenu_RefreshMarkers(s);
        Bg_SetLayerMap(1, s->layer, s->entries[s->index].map);
        dx = (s16)s->scrollX - (s16)s->originX;
        dy = (s16)s->scrollY - (s16)s->originY;
        tx = dx >> 3;
        ty = (dy >> 3) & ~1;
        BgGfx_LoadTilemapSimple(1, tx, ty, dx, dy);
        MapMenu_DrawActiveMarkers(s, tx, ty);
    } else {
        MapScreen_ClearMapLayer(s);
        MapMenu_ResetMarkers(s);
    }
    MessageWindow_SetMessageTable(s->panel);
    MessageWindow_SetPalette(0xefda);
    MessageWindow_SetDisplayMode(1);
    SceneSequence_SelectEntryMessage(s);
    Hud_SetInteractionCallback(s, MapMenu_UpdateInput, 0);
}
