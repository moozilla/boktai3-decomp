#include "global.h"
struct WorldPosition {
    s32 x, y, z;
};
struct ScreenPosition {
    s16 x, y, z, unused;
};
struct ActorDisplay {
    u8 pad0[0xB3];
    u8 screenSpace;
    u8 padB4[0x100 - 0xB4];
    void (*callback)(void);
    u8 pad104[0x268 - 0x104];
    struct WorldPosition position;
    u8 pad274[4];
    struct ScreenPosition screen;
    u8 pad280[0x288 - 0x280];
    s32 verticalOffset;
    u8 pad28C[0x2A0 - 0x28C];
    s32 limit;
    u8 pad2A4[0x5E0 - 0x2A4];
    s16 originX;
    u8 pad5E2[0x5EE - 0x5E2];
    s16 alternateLayer;
};
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void sub_081EC7D8(struct ActorDisplay *);
void sub_081EC974(struct ActorDisplay *);
void ActorDisplay_RefreshScreenLayer(struct ActorDisplay *actor)
{
    if (actor->alternateLayer) {
        s32 x = actor->screen.x;
        s32 origin = actor->originX;
        s32 layerX = origin + 160;
        s32 layerY;
        layerX -= x;
        layerY = origin + 88;
        BgGfx_LoadTilemapAtPosition(1, layerX, layerY - actor->verticalOffset);
    } else {
        s32 x = actor->screen.x;
        s32 origin = actor->originX;
        s32 layerX = origin + 160;
        s32 layerY;
        layerX -= x;
        layerY = origin + 88;
        BgGfx_LoadTilemapAtPosition(3, layerX, layerY - actor->verticalOffset);
    }
    sub_081EC7D8(actor);
    sub_081EC974(actor);
}
