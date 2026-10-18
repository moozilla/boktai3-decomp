/* Provisional names and object interpretation; offsets follow static accesses. */
#include "global.h"
struct MapMenuMarker {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x14];
    u16 x, y, z;
    u8 pad26[0x3a];
};
struct MapMenu {
    u8 pad0[0x64];
    u8 arrowData[0x20];
    u8 markerData[0x2b60 - 0x84];
    struct MapMenuMarker arrow;
    u8 pad2bc0[0x3820 - 0x2bc0];
    struct MapMenuMarker current, other;
    u8 pad38e0[0x4908 - 0x38e0];
    s32 adjacent;
    u16 scrollX, scrollY;
    u8 pad4910[8];
    u16 originX, originY, map, x, y, otherMap, otherX, otherY;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void sub_0816BC54(struct MapMenu *);
u32 MapScreen_GetPositionMarkerKeyframe(struct MapMenu *);
void MapMenu_RefreshMarkers(struct MapMenu *s)
{
    sub_0816BC54(s);
    if (s->adjacent != -1) {
        Anim_LoadKeyframe(&s->arrow, s->arrowData, 0x72, 1);
        s->arrow.flags &= ~1;
        s->arrow.x = 0;
        s->arrow.y = 0;
        s->arrow.z = 0;
    }
    s->current.flags &= ~1;
    Anim_LoadKeyframe(&s->current, s->markerData, (u16)MapScreen_GetPositionMarkerKeyframe(s), 1);
    s->current.x = s->x - (s->scrollX - s->originX);
    s->current.y = s->y - (s->scrollY - s->originY);
    s->current.z = 0;
    if (s->otherMap == s->map) {
        s->other.flags &= ~1;
        s->other.x = s->otherX - (s->scrollX - s->originX);
        s->other.y = s->otherY - (s->scrollY - s->originY);
        s->other.z = 0;
    }
}
