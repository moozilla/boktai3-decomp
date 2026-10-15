// MATCHING_CONCESSION: register-pinning
#include "scene_actor_route.h"
#include "script_utility.h"
#define SceneActorRoute_AdvanceActorPoint sub_08074698
void sub_0807E7DC(struct SceneActorRegistryActor *);
extern const u8 gUnk_08610814[];
void SceneActorRoute_AdvanceActorPoint(struct SceneActorRegistryActor *actor,
                                       struct SceneActorRouteCursor *cursor)
{
    struct SceneActorRouteCursor *embedded = (void *)((u8 *)actor + 0x448);
    u16 script = *(u16 *)((u8 *)actor + 0x1ec);
    u32 mode, near;
    if (script) {
        s32 values[3];
        struct ScriptUtilityArguments args;
        values[0] = embedded->pointIndex;
        values[1] = actor->id;
        values[2] = actor->category;
        /* The shared argument convention leaves the unused halfword unspecified. */
        args.count = 3;
        args.values = values;
        Script_Call(script, &args);
    }
    sub_0821E618(cursor);
    mode = cursor->point->attributes >> 4;
    near = (u8)sub_0821E8C8(cursor, &actor->position, 32, 32);
    if (near) {
        sub_0807E7DC(actor);
        {
            u32 behavior = 2;
            u32 offset = 0x2a8;
            register u8 *field asm("r0");
            u32 zero;
            register u32 phase asm("r3");
            field = (u8 *)actor + offset;
            zero = 0;
            *field = zero;
            phase = 0x2a9;
            field = (u8 *)actor + phase;
            *field = zero;
            phase++;
            field = (u8 *)actor + phase;
            *field = behavior;
            {
                u32 counter = 0x2ac;
                field = (u8 *)actor + counter;
                *(u32 *)field = zero;
            }
            phase += 7;
            {
                register u8 *entry asm("r1") = (u8 *)actor + phase;
                u32 one = 1;
                *entry = one;
            }
            {
                register u32 settingOffset asm("r0") = 0x2c2;
                u16 *setting;
                register u32 value asm("r0");
                asm("" : "+r"(settingOffset));
                setting = (void *)((u8 *)actor + settingOffset);
                value = 64;
                *setting = value;
            }
        }
    } else if (gUnk_08610814[mode] == 1) {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 1;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        {
            u32 *flags = &actor->flags2D0;
            u32 mask = ~0x20;
            u32 value;
            value = *flags;
            value &= mask;
            *flags = value;
        }
    } else {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
}
