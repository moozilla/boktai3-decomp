#include "global.h"
#include "scene_actor_reset.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
struct ActorTrail {
    u8 unknown0[0x8D6];
    s16 timer;
    u8 unknown8D8[0x1C];
    u16 soundDelay;
};
static inline u32 stateFlag(struct SceneActorRegistryResetView *p, u32 mask) { return p->flags2D4 & mask; }
static inline u32 actorFlag(struct SceneActorRegistryResetView *p, u32 mask) { return p->flags2D0 & mask; }
static inline u8 unavailable(struct SceneActorRegistryResetView *p) {
    if (p && (stateFlag(p, 0x40) || actorFlag(p, 0x20000) || p->health <= 0)) return 1;
    return 0;
}
void ActorTrail_UpdateTimedScale(struct SceneActorRegistryResetView *actor) {
    struct ActorTrail *trail;
    if (unavailable(actor)) return;
    trail = actor->context;
    if (trail->timer > 0) {
        if (trail->timer <= 59) actor->field2C2 = 32;
        else {
            if (*(s16 *)&actor->field2EA == 0 && actor->secondaryMotion.y == 0) {
                if (trail->soundDelay) trail->soundDelay--;
                else if (Mod(trail->timer, 24) == 0) Sound_PlaySong(984);
            }
            actor->field2C2 = Div(64, 3);
        }
    } else actor->field2C2 = 64;
}
