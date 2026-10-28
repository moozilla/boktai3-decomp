// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
#include "player.h"
s32 Mod(s32, s32);
static inline u8 Tick(void)
{
    u8 *player = (void *)gPlayers[0];
    register u32 offset asm("r3") = 0x4F4;
    asm("" : "+r"(offset) : "r"(player));
    return *(player + offset);
}
void SceneActorControl_UpdateTimedFacing(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    register struct SceneActorControlState *control asm("r5") = SceneActorControl_GetState(actor);
    s32 elapsed;
    s32 now;
    s32 start;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        control->startTick = Tick();
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        {
            u32 direction = actor->direction;
            register u32 offset asm("r3") = 0x3DD;
            {
                u8 *field = (u8 *)control + offset;
                asm("" : "+r"(field) : "r"(offset));
                *field = direction;
            }
        }
    }
    if (!Mod(*(u16 *)((u8 *)actor + 0x2CA), control->pulseInterval)) {
        register u32 offset asm("r3") = 0x188;
        register struct ContactCollider *collider asm("r2") = (void *)((u8 *)actor + offset);
        asm("" : "+r"(collider) : "r"(offset));
        collider->flags &= 0xFFFB;
    }
    start = control->startTick;
    now = Tick();
    if (now < start) {
        register s32 wrap asm("r3") = -256;
        {
            s32 adjusted = start + wrap;
            asm("" : "+r"(adjusted) : "r"(wrap));
            elapsed = now - adjusted;
        }
    } else
        elapsed = now - start;
    control->elapsed = elapsed;
    {
        register u32 limitOffset asm("r3") = 0x3DC;
        u8 *limitField = (u8 *)control + limitOffset;
        u8 limit;
        asm("" : "+r"(limitField) : "r"(limitOffset));
        limit = *limitField;
        if ((u8)elapsed > limit) {
            u32 offset = 0x2B0;
            u8 *field = (u8 *)actor + offset;
            asm("" : "+r"(field) : "r"(offset));
            *field = 1;
        } else {
            register u32 offset asm("r1") = 0x3DD;
            register u8 *saved asm("r6") = (u8 *)control + offset;
            u8 *direction;
            asm("" : "+r"(saved) : "r"(offset));
            direction = &actor->direction;
            {
                u32 prior = *saved;
                u32 current;
                current = *direction;
                if (prior != current) {
                    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
                    *saved = *direction;
                } else {
                    register u32 endedOffset asm("r3") = 0x2B3;
                    register u8 *field asm("r1") = (u8 *)actor + endedOffset;
                    asm("" : "+r"(field) : "r"(endedOffset));
                    *field = 0;
                }
            }
        }
    }
}
