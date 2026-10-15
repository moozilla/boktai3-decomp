// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32);
void sub_0807FC90(u8 *);
static inline u8 HasMovementFlag(struct SceneActorRegistryActor *actor)
{
    u32 mask = 0x100;
    u32 offset = 0x2c0;
    u16 *field;
    field = (void *)((u8 *)actor + offset);
    if (*field & mask)
        return 1;
    return 0;
}
void SceneActorRegistry_RequestScriptState(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    u32 preserveControl = Script_GetKeywordValue('m', 0);
    if (id) {
        struct SceneActorRegistryActor *actor = SceneActorRegistry_FindById(id);
        if (actor && HasMovementFlag(actor)) {
            u32 state = 2;
            register u32 offset asm("r4") = 0x2a8;
            register u8 *field asm("r0");
            register u32 zero asm("r1");
            u32 phaseOffset;
            field = (u8 *)actor + offset;
            zero = 0;
            *field = state;
            phaseOffset = 0x2a9;
            field = (u8 *)actor + phaseOffset;
            *field = zero;
            {
                u32 behaviorOffset = 0x2aa;
                field = (u8 *)actor + behaviorOffset;
                *field = zero;
            }
            offset += 4;
            asm("" : "+r"(offset));
            field = (u8 *)actor + offset;
            *(u32 *)field = zero;
            phaseOffset += 8;
            field = (u8 *)actor + phaseOffset;
            {
                u32 one = 1;
                *field = one;
                if (!preserveControl) {
                    const void *control = (const void *)sub_0807FC90;
                    u32 substate = 16;
                    phaseOffset += 1;
                    field = (u8 *)actor + phaseOffset;
                    *field = one;
                    {
                        u32 startOffset = 0x2b0;
                        field = (u8 *)actor + startOffset;
                        *field = preserveControl;
                    }
                    phaseOffset += 3;
                    field = (u8 *)actor + phaseOffset;
                    *field = substate;
                    {
                        u32 controlOffset = 0x21c;
                        field = (u8 *)actor + controlOffset;
                        *(const void **)field = control;
                    }
                    {
                        u32 counterOffset = 0x2c8;
                        field = (u8 *)actor + counterOffset;
                        *(u16 *)field = preserveControl;
                    }
                    {
                        u32 flagOffset = 0x114;
                        u32 *flags;
                        register u32 mask asm("r1");
                        u32 value;
                        flags = (void *)((u8 *)actor + flagOffset);
                        mask = ~1;
                        value = *flags;
                        value &= mask;
                        *flags = value;
                    }
                    {
                        register u32 clearOffset asm("r6") = 0x104;
                        asm("" : "+r"(clearOffset));
                        field = (u8 *)actor + clearOffset;
                        *(u32 *)field = preserveControl;
                    }
                }
            }
            {
                register u32 mask asm("r0") = 0x4000;
                register u32 flagOffset asm("r1") = 0x2d4;
                register u16 *flags asm("r2");
                u32 value;
                flags = (void *)((u8 *)actor + flagOffset);
                value = *flags;
                mask |= value;
                *flags = mask;
            }
            {
                u32 mask = 8;
                register u32 flagOffset asm("r4") = 0x2d0;
                register u32 *flags asm("r1");
                u32 value;
                flags = (void *)((u8 *)actor + flagOffset);
                value = *flags;
                value |= mask;
                *flags = value;
            }
        }
    }
}
