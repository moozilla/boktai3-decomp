// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "player.h"
extern const s16 gUnk_086149C4[];
void CpuSet(const void *, void *, u32);
void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *);
void SceneActorControl_StartPositionCurve(struct SceneActorRegistryResetView *,
                                          const struct ContactPosition *);
static inline s32 Scale(s32 v)
{
    return v >= 0 ? v >> 7 : -(-v >> 7);
}
void SceneActorControl_TickRandomMotion(struct SceneActorRegistryResetView *input)
{
    register struct SceneActorRegistryResetView *actor asm("r5") = input;
    register struct SceneActorControlState *control asm("r4") = SceneActorControl_GetState(actor);
    u32 zero;

    u32 magnitude;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        register void *callback = actor->entryTarget228;
        u32 index;
        {
            u32 one = 1;
            register u32 offset = 0x2B2;
            register u32 clear;
            u8 *field;
            field = (u8 *)actor + offset;
            clear = 0;
            *field = one;
            offset -= 2;
            *((u8 *)actor + offset) = clear;
            offset += 5;
            field = (u8 *)actor + offset;
            *field = one;
            actor->target = callback;
            actor->field2C8 = 0;
            asm("" ::"r"(offset));
            offset = 0x114;

            {
                register u32 *flags = (void *)((u8 *)actor + offset);
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = 0;
            asm("" ::"r"(offset));
        }
        {
            void *destination = (u8 *)control + 0x42C;
            zero = 0;
            {
                u32 *source = &zero;
                u32 control = 0x05000002;
                asm("" : "+r"(source));
                asm("" : "+r"(control));
                CpuSet(source, destination, control);
            }
        }
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        index *= 2;
        {
            register const u16 *table asm("r3") = gUnk_0203B400;
            u32 value = *(u16 *)(index + (u32)table);
            register u32 offset = 0x3EC;

            {
                register u8 *field asm("r1") = (u8 *)control + offset;
                *field = value;
                {
                    register const s16 *sine = gUnk_086149C4;
                    s32 sample;
                    sample = sine[*field];
                    asm("" ::"r"(offset));
                    actor->secondaryMotion.x = Scale(sample);
                    asm("" ::"r"(table));
                }
            }
        }
        {
            register const s16 *sine asm("r2") = gUnk_086149C4;
            register u32 offset = 0x3EC;
            u8 *field = (u8 *)control + offset;
            u32 angle = *field + 64;
            register u32 mask = 255;
            s32 sample;
            angle &= mask;
            sample = sine[angle];
            asm("" ::"r"(offset), "r"(mask), "r"(sine));
            actor->secondaryMotion.z = Scale(sample);
        }
    }
    if ((s32)actor->counter2AC <= 5) {
        const s16 *first = gUnk_086149C4;
        register u32 offset asm("r6") = 0x3EC;
        u8 *field;
        const s16 *table;
        s32 value;

        field = (u8 *)control + offset;
        value = first[*field];
        table = first;
        actor->secondaryMotion.x = Scale(value);
        offset = 0x3EC;

        field = (u8 *)control + offset;
        value = table[(*field + 64) & 255];
        actor->secondaryMotion.z = Scale(value);
    }
    {
        struct ContactPosition *motion = &actor->secondaryMotion;
        s32 x = motion->x;
        u32 squareX = (u32)x * x;
        {
            s32 z = motion->z;
            u32 squareZ = (u32)z * z;
            {
                u32 value = squareZ;
                magnitude = squareX + value;
            }
        }
        if (!magnitude) {
            SceneActorControl_ResetCollisionPhase(actor);
            zero = magnitude;
            {
                register u32 *source = &zero;
                {
                    register void *destination = motion;
                    asm("" : "+r"(source), "+r"(destination));
                    CpuSet(source, destination, 0x05000002);
                }
            }
            SceneActorControl_StartPositionCurve(
                actor, (const struct ContactPosition *)&gUnk_02000580[0]->position);
            {
                register u32 phase = 2;
                register u32 stateOffset asm("r2") = 0x2A8;
                register u32 phaseOffset asm("r3");
                register u32 behaviorOffset;
                u8 *field = (u8 *)actor + stateOffset;
                *field = magnitude;
                phaseOffset = 0x2A9;
                *((u8 *)actor + phaseOffset) = phase;
                behaviorOffset = 0x2AA;
                *((u8 *)actor + behaviorOffset) = magnitude;
                stateOffset += 4;
                *(u32 *)((u8 *)actor + stateOffset) = magnitude;
                phaseOffset += 8;
                {
                    register u8 *field asm("r1") = (u8 *)actor + phaseOffset;
                    *field = 1;
                }
            }
        }
    }
    {
        register u32 offset asm("r6") = 0x2AC;
        register u32 *counter asm("r1") = (void *)((u8 *)actor + offset);
        asm("" : "+r"(offset), "+r"(counter));
        ++*counter;
    }
}
