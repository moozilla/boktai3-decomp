/* Provisional names and object interpretation; offsets follow static accesses. */
#include "global.h"
#include "ui/pause/scene_hud_display.h"
u32 MapMenu_SelectDirection(struct SceneHudState *);
void MenuSequence_SelectEntryMessage(struct SceneHudState *);
void MapMenu_ClampScrollAndUpdateArrows(struct SceneHudState *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void MapMenu_DrawActiveMarkers(struct SceneHudState *, s32, s32);
void MapScreen_StepDirectionalIndicators(struct SceneHudState *);
void MapMenu_UpdateInput(struct SceneHudState *s)
{
    u32 changed = 0;
    if (gPadState[0] & 1) {
        changed = MapMenu_SelectDirection(s);
        if (changed) {
            Sound_PlaySong(0xdc);
            if (s->navigationPage == s->currentMapPage)
                s->mapPositionMarkers[0].flags &= ~1;
            else
                s->mapPositionMarkers[0].flags |= 1;
            if (s->navigationPage == s->otherMapPage)
                s->mapPositionMarkers[1].flags &= ~1;
            else
                s->mapPositionMarkers[1].flags |= 1;
            MenuSequence_SelectEntryMessage(s);
        }
    } else if (s->showAllMapLinks) {
        if (gPadState[0] & 0x20) {
            s->mapScrollX -= 2;
            changed = 1;
        } else if (gPadState[0] & 0x10) {
            s->mapScrollX += 2;
            changed = 1;
        }
        if (gPadState[0] & 0x40) {
            s->mapScrollY -= 2;
            changed = 1;
        } else if (gPadState[0] & 0x80) {
            s->mapScrollY += 2;
            changed = 1;
        }
    }
    if (s->showAllMapLinks && changed) {
        s32 dx, dy, tx, ty;
        MapMenu_ClampScrollAndUpdateArrows(s);
        dx = (s16)s->mapScrollX - (s16)s->mapFocusX;
        dy = (s16)s->mapScrollY - (s16)s->mapFocusY;
        tx = dx >> 3;
        ty = (dy >> 3) & ~1;
        BgGfx_LoadTilemapSimple(1, tx, ty, dx, dy);
        MapMenu_DrawActiveMarkers(s, tx, ty);
        if (!(s->mapPositionMarkers[0].flags & 1)) {
            s->mapPositionMarkers[0].position.x = s->mapPositionX - dx;
            s->mapPositionMarkers[0].position.y = s->mapPositionY - dy;
        }
        if (!(s->mapPositionMarkers[1].flags & 1)) {
            s->mapPositionMarkers[1].position.x = s->otherMapX - dx;
            s->mapPositionMarkers[1].position.y = s->otherMapY - dy;
        }
    }
    MapScreen_StepDirectionalIndicators(s);
}
