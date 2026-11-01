#include "scene_departure_display.h"
#include "background_controller.h"
#include "ram.h"

extern const u16 gUnk_086141BC[];
s32 Player_GetHorizontalVelocity(s32);
struct Camera {
    u8 unknown0[0x44];
    s32 x;
};

static inline void GetPlayerCoords(struct ScenePoolPlayer *p, struct ScenePoolVec32 *out)
{
    if (!p) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
    } else {
        out->x = p->position[0];
        out->y = p->position[1];
        out->z = p->position[2];
    }
}
void SceneActors_UpdatePlayerFollowingDisplay(struct DepartureActor *input, s32 time)
{
    struct DepartureActor *actor = input;
    struct ScenePoolVec32 player;
    s32 distance, speed;
    DepartureActor_EnterDisplay(actor);
    GetPlayerCoords(gParticipantActors[0], &player);
    distance = (player.x - actor->position.x) >> 12;
    speed = Player_GetHorizontalVelocity(actor->playerIndex);
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    {
        u32 sample = gUnk_086141BC[gEffectRandomTableIndex];
        actor->jitter = sample & 1;
    }
    if (distance < 0) {
        actor->acceleration = -128;
        actor->motionState = 1;
    } else if (distance < 64 && (u32)(actor->motionState - 2) > 1) {
        if (speed > actor->velocity)
            actor->motionState = 2;
        else
            actor->motionState = 3;
    } else if (distance > 64) {
        actor->acceleration = 128;
        actor->motionState = 0;
    }
    switch (actor->motionState) {
        case 0:
        case 1:
            actor->velocity += actor->acceleration;
            if (actor->velocity > 0x6000)
                actor->velocity = 0x6000;
            if (actor->velocity < 0x2000)
                actor->velocity = 0x2000;
            break;
        case 2:
            actor->acceleration = 128;
            actor->velocity += actor->acceleration;
            if (actor->velocity > speed) {
                actor->velocity = speed;
                actor->acceleration = 0;
                actor->motionState = 4;
            }
            break;
        case 3:
            actor->acceleration = -128;
            actor->velocity += actor->acceleration;
            if (actor->velocity < speed) {
                actor->velocity = speed;
                actor->acceleration = 0;
                actor->motionState = 4;
            }
            break;
        case 4:
            break;
    }
    actor->position.x += actor->velocity;
    actor->position.z = actor->fixedZ;
    {
        struct SceneVec *screen = &actor->screen;
        struct ScenePoolVec32 *pos = &actor->position;
        {
            s32 x = pos->x >> 12;
            struct Camera *cam = gSceneCamera;
            screen->x = x - cam->x;
        }
        {
            s32 y = pos->y;
            s32 z = pos->z;
            screen->y = ((z + y) >> 12) + 96;
        }
        screen->z = pos->z >> 12;
    }
    if (actor->screen.x > 160) {
        actor->position.x -= actor->velocity;
        actor->velocity = 0;
    }
    ActorDisplay_UpdateScreenAnchor(input);
    ActorDisplay_UpdateForegroundSprite(input);
    ActorDisplay_UpdateAccentSprite(input);
    SceneActors_EmitPeriodicParticles(input, time);
}
