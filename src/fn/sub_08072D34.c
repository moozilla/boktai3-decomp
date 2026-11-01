// MATCHING_CONCESSION: register-pinning
#include "scene_actor_contact_list.h"
static inline u8 Unavailable(struct SceneActorRegistryActor *inputActor, u16 *inputState,
                             u32 inputDisabled, u32 inputExcluded)
{
    register struct SceneActorRegistryActor *actor asm("r3") = inputActor;
    u32 disabled = inputDisabled;
    u16 *state = inputState;
    u32 excluded = inputExcluded;
    u32 flags;
    flags = *state;
    if ((flags & disabled) || (actor->flags2D0 & excluded) || actor->health <= 0)
        return 1;
    return 0;
}
static inline u32 InvalidFlags(struct SceneActorRegistryActor *actor)
{
    u32 *field = &actor->flags2CC;
    u32 value = *field;
    asm("" : : "r"(actor));
    return value;
}
void SceneActorContacts_ReleaseInvalidOwners(void)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *node;
    if (!manager || Mod(manager->frame, 30))
        return;
    node = manager->head;
    if (!node)
        return;
    {
        u32 disabled = 0x40;
        u32 excluded = 0x20000;
        u32 required = 0x1000;
        u32 invalidMask = 3;
        u32 clearMask = 0xFD;
        do {
            u32 incoming = node->flags;
            u32 flags;
            register struct SceneActorRegistryActor *actor asm("r3");
            u16 *state;
            {
                u32 active = incoming & 1;
                flags = incoming;
                if (!active)
                    goto advance;
            }
            if (!(flags & 2))
                goto advance;
            actor = node->owner;
            if (!actor)
                goto invalid;
            {
                u32 offset = 0x2D4;
                state = (void *)((u32)actor + offset);
            }
            if (Unavailable(actor, state, disabled, excluded) ||
                (InvalidFlags(actor) & invalidMask)) {
            invalid: {
                u32 clear = clearMask;
                register u32 zero asm("r1");
                clear &= flags;
                zero = 0;
                asm("" : "+r"(zero) : "r"(clear));
                node->flags = clear;
                node->owner = (void *)zero;
                {
                    u32 cooldown = 120;
                    node->cooldown = cooldown;
                }
                continue;
            }
            }
            {
                register u32 value asm("r0") = *state;
                u32 mask = required;
                register u32 valid asm("r1");
                value &= mask;
                valid = (u16)value;
                if (!valid) {
                    u32 clear = clearMask;
                    asm("" : "+r"(clear));
                    clear &= flags;
                    node->flags = clear;
                    node->owner = (void *)valid;
                    {
                        register u32 cooldown asm("r0") = 120;
                        node->cooldown = cooldown;
                    }
                }
            }
        advance:
            node = node->next;
        } while (node);
    }
}
