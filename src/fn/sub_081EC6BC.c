#include "global.h"

#define gActorDisplayCamera gUnk_020004B8
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
struct DisplayCamera {
    u8 pad[0x44];
    s32 x;
};
struct PackedAnchor {
    s16 x, y, z;
};
extern struct DisplayCamera *gUnk_020004B8;
void SceneActors_UpdateDelayedMovement(void);
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void ActorDisplay_UpdateScreenAnchor(struct ActorDisplay *actor)
{
    if (!actor->screenSpace) {
        struct WorldPosition *position = &actor->position;
        struct ScreenPosition *screen;
        s32 threshold = 0;
        s32 limit = (actor->limit - 180) << 12;
        if (limit != 0)
            threshold = actor->callback != SceneActors_UpdateDelayedMovement;
        if (position->x < threshold)
            position->x = limit;
        screen = &actor->screen;
        screen->x = (position->x >> 12) - gUnk_020004B8->x;
        {
            s32 y = position->y;
            s32 z = position->z;
            screen->y = ((z + y) >> 12) + 96;
        }
        screen->z = position->z >> 12;
    } else {
        struct PackedAnchor anchor;
        anchor.x = actor->position.x;
        anchor.y = actor->position.y;
        anchor.z = 0;
        actor->screen.x = anchor.x;
        actor->screen.y = 0;
    }
    if (actor->alternateLayer) {
        s32 x = actor->screen.x;
        s32 origin = actor->originX + 160;
        BgGfx_LoadTilemapAtPosition(1, origin - x, 88 - actor->verticalOffset);
    } else {
        s32 x = actor->screen.x;
        s32 origin = actor->originX + 160;
        BgGfx_LoadTilemapAtPosition(3, origin - x, 88 - actor->verticalOffset);
    }
}
