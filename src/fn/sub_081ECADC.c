#include "scene_departure_display.h"

void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void ActorDisplay_RefreshScreenLayer(struct DepartureActor *actor)
{
    if (actor->cameraActive) {
        s32 x = actor->screen.x;
        s32 origin = actor->shakeStrength;
        s32 layerX = origin + 160;
        s32 layerY;
        layerX -= x;
        layerY = origin + 88;
        BgGfx_LoadTilemapAtPosition(1, layerX, layerY - actor->jitter);
    } else {
        s32 x = actor->screen.x;
        s32 origin = actor->shakeStrength;
        s32 layerX = origin + 160;
        s32 layerY;
        layerX -= x;
        layerY = origin + 88;
        BgGfx_LoadTilemapAtPosition(3, layerX, layerY - actor->jitter);
    }
    ActorDisplay_UpdateForegroundSprite(actor);
    ActorDisplay_UpdateAccentSprite(actor);
}
