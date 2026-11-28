#include "npc/manager/scene_actor_actions.h"
#include "system/clock/sun_clock.h"

int Weather_GetType(void);

int SceneActorManager_Update(struct SceneActorManager *manager)
{
    struct Player *player = gPlayers[0];
    struct SceneActor *actor;

    manager->player = player;
    if (player)
        manager->target = (struct ContactPosition *)&player->position;
    else
        manager->target = 0;
    manager->weather = Weather_GetType();
    manager->hour = SunClock_GetBaseHour();
    manager->minute = SunClock_GetBaseMinute();
    if (manager->previousMinute != manager->minute)
        manager->minuteChanged = 1;
    else
        manager->minuteChanged = 0;
    manager->previousMinute = manager->minute;
    manager->buffer = 1 - manager->buffer;
    manager->count[manager->buffer] = 0;
    actor = manager->head;
    while (actor) {
        struct SceneActor *previous = actor->previous;
        Actor_RunCallbacks(manager, actor);
        actor = previous;
    }
    Actor_UpdateInteractionCue(manager);
    if (manager->player)
        Actor_EmitTowardNearbyActors(
            (struct SceneActorBody *)&manager->player->positionalEffectGroup);
    manager->ticks++;
    return 0;
}
