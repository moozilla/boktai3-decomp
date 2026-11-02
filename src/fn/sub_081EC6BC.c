#include "scene_departure_display.h"
#include "background_controller.h"

struct DisplayCamera {
    u8 pad[0x44];
    s32 x;
};
struct PackedAnchor {
    s16 x, y, z;
};
void SceneActors_UpdateDelayedMovement(u8 *, s32);
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void ActorDisplay_UpdateScreenAnchor(struct DepartureActor *actor)
{
    if (!actor->motionMode) {
        struct ScenePoolVec32 *position = &actor->position;
        struct SceneVec *screen;
        s32 threshold = 0;
        s32 limit = (actor->screenLimit - 180) << 12;
        if (limit != 0)
            threshold = (void (*)(u8 *, s32))actor->callback != SceneActors_UpdateDelayedMovement;
        if (position->x < threshold)
            position->x = limit;
        screen = &actor->screen;
        screen->x = (position->x >> 12) - ((struct DisplayCamera *)gSceneCamera)->x;
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
    if (actor->cameraActive) {
        s32 x = actor->screen.x;
        s32 origin = actor->shakeStrength + 160;
        BgGfx_LoadTilemapAtPosition(1, origin - x, 88 - actor->jitter);
    } else {
        s32 x = actor->screen.x;
        s32 origin = actor->shakeStrength + 160;
        BgGfx_LoadTilemapAtPosition(3, origin - x, 88 - actor->jitter);
    }
}
