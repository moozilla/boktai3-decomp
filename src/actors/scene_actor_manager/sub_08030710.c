#include "engine/shared/scene_actor_actions.h"
#include "system/clock_and_sun_schedule/sun_clock.h"

int Weather_GetType(void);

int SceneActorManager_Update(struct SceneActorManager *m)
{
    struct Player *player = gPlayers[0];
    m->player = player;
    if (player) {
        m->target = (struct ContactPosition *)&player->position;
    } else {
        m->target = 0;
    }
    m->weather = Weather_GetType();
    m->hour = SunClock_GetBaseHour();
    m->minute = SunClock_GetBaseMinute();
    if (m->previousMinute != m->minute)
        m->minuteChanged = 1;
    else
        m->minuteChanged = 0;
    m->previousMinute = m->minute;
    {
        u32 buffer = 1 - m->buffer;
        m->buffer = buffer;
        m->count[buffer] = 0;
    }
    {
        struct SceneActor *p = m->head;
        while (p) {
            struct SceneActor *prev = p->previous;
            Actor_RunCallbacks(m, p);
            p = prev;
        }
    }
    Actor_UpdateInteractionCue(m);
    if (m->player) {
        Actor_EmitTowardNearbyActors((struct SceneActorBody *)&m->player->positionalEffectGroup);
    }
    m->ticks++;
    return 0;
}
