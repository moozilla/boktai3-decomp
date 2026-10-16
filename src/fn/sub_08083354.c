#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.4%
// MATCHING_CONCESSION: register-pinning
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
static inline void ResetPhase(struct SceneActorRegistryResetView *actor, u32 zero)
{
    actor->state2A8 = zero;
    actor->phase2A9 = 2;
    actor->behavior2AA = zero;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}
void SceneActorControl_TickRandomMotion(struct SceneActorRegistryResetView *input)
{
    register struct SceneActorRegistryResetView *actor asm("r5") = input;
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    u32 zero;

    register u32 magnitude asm("r6");
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = *(void **)((u8 *)actor + 0x228);
        u32 index;
        {
            u32 one = 1;
            u32 offset = 0x2B2;
            u32 clear;
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
            offset = 0x114;
            {
                u32 *flags = (void *)((u8 *)actor + offset);
                register u32 mask asm("r1") = ~1;
                asm("" : "+r"(mask) : "r"(flags), "r"(offset));
                *flags &= mask;
            }
            actor->word104 = 0;
        }
        {
            void *destination = (u8 *)control + 0x42C;
            asm("" : "+r"(destination));
            zero = 0;
            {
                register u32 *source asm("r0") = &zero;
                CpuSet(source, destination, 0x05000002);
            }
        }
        index = (gUnk_03005308 + 1) & 1023;
        gUnk_03005308 = index;
        index *= 2;
        {
            const u16 *table = gUnk_0203B400;
            u32 value = *(u16 *)(index + (u32)table);
            register u32 offset asm("r6") = 0x3EC;
            {
                register u8 *field asm("r1") = (u8 *)control + offset;
                asm("" : "+r"(field) : "r"(offset));
                *field = value;
                {
                    const s16 *sine = gUnk_086149C4;
                    actor->secondaryMotion.x = Scale(sine[*field]);
                    asm("" : : "r"(table));
                }
            }
        }
        actor->secondaryMotion.z = Scale(gUnk_086149C4[(*((u8 *)control + 0x3EC) + 64) & 255]);
    }
    if ((s32)actor->counter2AC <= 5) {
        const s16 *first = gUnk_086149C4;
        register u32 offset asm("r6") = 0x3EC;
        u8 *field = (u8 *)control + offset;
        const s16 *table;
        s32 value;
        asm("" : "+r"(field) : "r"(offset));
        value = first[*field];
        table = first;
        asm("" : "+r"(table) : "r"(value));
        actor->secondaryMotion.x = Scale(value);
        offset = 0x3EC;
        field = (u8 *)control + offset;
        asm("" : "+r"(field) : "r"(offset));
        actor->secondaryMotion.z = Scale(table[(*field + 64) & 255]);
    }
    {
        struct ContactPosition *motion = &actor->secondaryMotion;
        s32 x = motion->x;
        register u32 squareX asm("r1") = (u32)x * x;
        {
            s32 z = motion->z;
            u32 squareZ = (u32)z * z;
            {
                register u32 value asm("r0") = squareZ;
                magnitude = squareX + value;
            }
        }
        if (!magnitude) {
            SceneActorControl_ResetCollisionPhase(actor);
            zero = magnitude;
            {
                register u32 *source asm("r0") = &zero;
                {
                    register void *destination asm("r1") = motion;
                    CpuSet(source, destination, 0x05000002);
                }
            }
            SceneActorControl_StartPositionCurve(
                actor, (const struct ContactPosition *)&gUnk_02000580[0]->position);
            {
                u32 phase = 2;
                register u32 stateOffset asm("r2") = 0x2A8;
                register u32 phaseOffset asm("r3");
                u32 behaviorOffset;
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
        asm("" : "+r"(counter) : "r"(offset));
        ++*counter;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorControl_TickRandomMotion);
#endif
