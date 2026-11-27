/* Provisional names and object interpretation; offsets follow static accesses. */
#include "global.h"
#include "ui/pause/map_menu_marker.h"
struct MapMenu {
    u8 pad0[0x64];
    u8 arrowData[0x20];
    u8 markerData[0x2b60 - 0x84];
    struct MapMenuMarkerSprite arrow;
    u8 pad2bc0[0x3820 - 0x2bc0];
    struct MapMenuMarkerSprite current, other;
    u8 pad38e0[0x4908 - 0x38e0];
    s32 adjacent;
    u16 scrollX, scrollY;
    u8 pad4910[8];
    u16 originX, originY, map, x, y, otherMap, otherX, otherY;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void MapMenu_RefreshNavigationArrows(struct MapMenu *);
u32 MapScreen_GetPositionMarkerKeyframe(struct MapMenu *);
void MapMenu_ResetMarkers(struct MapMenu *s)
{
    MapMenu_RefreshNavigationArrows(s);
    if (s->adjacent != -1) {
        Anim_LoadKeyframe(&s->arrow, s->arrowData, 0x72, 1);
        s->arrow.flags &= ~1;
        s->arrow.x = 0;
        s->arrow.y = 0;
        s->arrow.z = 0;
    }
    s->current.flags &= ~1;
    Anim_LoadKeyframe(&s->current, s->markerData, 3, 1);
    s->current.x = 0;
    s->current.y = 0;
    s->current.z = 0;
    s->other.flags |= 1;
}
