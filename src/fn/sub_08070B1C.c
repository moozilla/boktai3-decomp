// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorControl_OnIdle(u8 *);
static inline u8 IsUnavailable(struct SceneActorRegistryActor *input, u16 **saved,
                               u32 excludedInput)
{
    register struct SceneActorRegistryActor *actor asm("r3") = input;
    register u32 excluded asm("r10") = excludedInput;
    u32 offset = 0x2d4;
    u16 *field;
    register u32 value asm("r0");
    u32 mask;
    field = (void *)((u8 *)actor + offset);
    value = *field;
    mask = 0x40;
    value &= mask;
    *saved = field;
    if (value)
        goto one;
    {
        u32 offset = 0x2d0;
        u32 *field;
        field = (void *)((u8 *)actor + offset);
        value = *field;
        mask = excluded;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
void SceneActorRegistry_ResetEligibleCategoryState(void)
{
    struct SceneActorRegistryNode *node;
    register struct SceneActorRegistryActor *actor asm("r3");
    SceneActorRegistry_GetManager();
    node = gSceneActorRegistryNode;
    actor = node->actor;
    if (actor) {
        register u32 excluded asm("r10") = 0x20000;
        u32 reject;
        const void *callback;
        u32 substate;
        u32 one;
        u32 zero;
        {
            u32 initial = 0xa007;
            reject = initial;
        }
        {
            register const void *initial asm("r2") = (const void *)SceneActorControl_OnIdle;
            asm("" : "+r"(initial));
            callback = initial;
        }
        {
            register u32 initial asm("r0") = 16;
            asm("" : "+r"(initial));
            substate = initial;
        }
        do {
            u16 *flags;
            one = 1;
            zero = 0;
            {
                u32 unavailable = IsUnavailable(actor, &flags, excluded);
                if (!unavailable) {
                    u32 category = actor->category;
                    if (category == 4 || category == 1 || category == 3) {
                        u32 selected = *flags & reject;
                        if (!selected) {
                            u32 offset = 0x2b2;
                            register u8 *field asm("r0");
                            field = (u8 *)actor + offset;
                            *field = one;
                            {
                                u32 beginOffset = 0x2b0;
                                field = (u8 *)actor + beginOffset;
                                *field = selected;
                            }
                            offset += 3;
                            field = (u8 *)actor + offset;
                            {
                                u32 code = substate;
                                *field = code;
                            }
                            offset -= 0x99;
                            field = (u8 *)actor + offset;
                            {
                                const void *control = callback;
                                *(const void **)field = control;
                            }
                            offset += 0xac;
                            field = (u8 *)actor + offset;
                            *(u16 *)field = selected;
                            {
                                u32 flagOffset = 0x114;
                                u32 *flags;
                                u32 mask;
                                u32 value;
                                flags = (void *)((u8 *)actor + flagOffset);
                                mask = ~1;
                                value = *flags;
                                value &= mask;
                                *flags = value;
                            }
                            {
                                u32 clearOffset = 0x104;
                                field = (u8 *)actor + clearOffset;
                                *(u32 *)field = selected;
                            }
                            {
                                u32 stateOffset = 0x2a8;
                                register u32 phaseOffset asm("r1");
                                field = (u8 *)actor + stateOffset;
                                *field = zero;
                                phaseOffset = 0x2a9;
                                field = (u8 *)actor + phaseOffset;
                                *field = one;
                                stateOffset += 2;
                                field = (u8 *)actor + stateOffset;
                                *field = zero;
                                phaseOffset += 3;
                                field = (u8 *)actor + phaseOffset;
                                *(u32 *)field = selected;
                                stateOffset += 7;
                                field = (u8 *)actor + stateOffset;
                                *field = one;
                            }
                        }
                    }
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
