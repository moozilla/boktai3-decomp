/* Provisional names and object interpretation; offsets follow static accesses. */
#include "global.h"
struct MapMenu {
    u8 pad0[0x3828];
    u32 flagsA;
    u8 pad382c[0x3840 - 0x382c];
    s16 posAX, posAY;
    u8 pad3844[0x3888 - 0x3844];
    u32 flagsB;
    u8 pad388c[0x38a0 - 0x388c];
    s16 posBX, posBY;
    u8 pad38a4[0x4904 - 0x38a4];
    u8 index, unlocked;
    u8 pad4906[6];
    u16 scrollX, scrollY;
    u8 pad4910[8];
    u16 originX, originY;
    u16 map, x, y, otherMap, otherX, otherY;
};
extern struct {
    u16 held, pressed;
} gPadState;
u32 MapMenu_SelectDirection(struct MapMenu *);
void Sound_PlaySong(u32);
void SceneSequence_SelectEntryMessage(struct MapMenu *);
void MapMenu_ClampScrollAndUpdateArrows(struct MapMenu *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void MapMenu_DrawActiveMarkers(struct MapMenu *, s32, s32);
void MapScreen_StepDirectionalIndicators(struct MapMenu *);
void MapMenu_UpdateInput(struct MapMenu *s)
{
    u32 changed = 0;
    if (gPadState.held & 1) {
        changed = MapMenu_SelectDirection(s);
        if (changed) {
            Sound_PlaySong(0xdc);
            if (s->index == s->map)
                s->flagsA &= ~1;
            else
                s->flagsA |= 1;
            if (s->index == s->otherMap)
                s->flagsB &= ~1;
            else
                s->flagsB |= 1;
            SceneSequence_SelectEntryMessage(s);
        }
    } else if (s->unlocked) {
        if (gPadState.held & 0x20) {
            s->scrollX -= 2;
            changed = 1;
        } else if (gPadState.held & 0x10) {
            s->scrollX += 2;
            changed = 1;
        }
        if (gPadState.held & 0x40) {
            s->scrollY -= 2;
            changed = 1;
        } else if (gPadState.held & 0x80) {
            s->scrollY += 2;
            changed = 1;
        }
    }
    if (s->unlocked && changed) {
        s32 dx, dy, tx, ty;
        MapMenu_ClampScrollAndUpdateArrows(s);
        dx = (s16)s->scrollX - (s16)s->originX;
        dy = (s16)s->scrollY - (s16)s->originY;
        tx = dx >> 3;
        ty = (dy >> 3) & ~1;
        BgGfx_LoadTilemapSimple(1, tx, ty, dx, dy);
        MapMenu_DrawActiveMarkers(s, tx, ty);
        if (!(s->flagsA & 1)) {
            s->posAX = s->x - dx;
            s->posAY = s->y - dy;
        }
        if (!(s->flagsB & 1)) {
            s->posBX = s->otherX - dx;
            s->posBY = s->otherY - dy;
        }
    }
    MapScreen_StepDirectionalIndicators(s);
}
