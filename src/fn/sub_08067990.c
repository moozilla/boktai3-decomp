// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control.h"
#include "ram.h"
/* SceneActorControl_CreateFromScript: Creates an actor slot using script position and flip parameters and initializes its sprite and collider */
#define SceneActorControl_CreateFromScript sub_08067990
#define SceneActorControl_CreateFromScript sub_08067990
struct SceneActorControlManager *SceneActorControl_EnsureSingleton(void);
struct SceneActorControlSlot *SceneActorControl_AllocateSlot(struct SceneActorControlManager *);
s32 Region_FindContaining(const struct ContactPosition *);
void Sprite_InitUnlinked(void *, void *, u32);
u32 AreaEffect_HasEnabledTerrainValue(s32);
void SceneActorControl_InitCollider(struct SceneActorControlSlot *, s32, s32);
static inline u16 *Step(struct SceneActorControlSlot *slot,
                        struct SceneActorControlSprite *inputSprite, u16 *inputFlags)
{
    register struct SceneActorControlSprite *sprite asm("r8") = inputSprite;
    u16 *lateFlags;
    struct DisplaySequenceState *state = &slot->sequence;
    const volatile u16 *command =
        (const volatile u16 *)((u32)state->commands + (state->index << 1));
    u32 ended, duration, flags, bit;
    slot->tile = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        {
            u32 flip = ((*command & 0x30) >> 4) & bit;
            asm("" : "=r"(lateFlags) : "0"(inputFlags), "r"(flip));
            if (first != flip)
                slot->spriteFlags |= 4;
            else
                slot->spriteFlags &= ~4;
        }
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit)) {
            register struct SceneActorControlSprite *read asm("r1") = sprite;
            u32 value = read->flags;
            struct SceneActorControlSprite *write;
            value |= 8;
            write = sprite;
            write->flags = value;
        } else {
            register struct SceneActorControlSprite *read asm("r1") = sprite;
            u32 value = read->flags;
            struct SceneActorControlSprite *write;
            value &= ~8;
            write = sprite;
            write->flags = value;
        }
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return lateFlags;
}
void SceneActorControl_CreateFromScript(void)
{
    struct SceneActorControlManager *p = gSceneActorControlManager;
    struct SceneActorControlSlot *slot;
    u8 flip;
    register struct SceneActorControlSprite *sprite asm("r8");
    register u16 *slotFlags asm("r9");
    if (!p) {
        p = SceneActorControl_EnsureSingleton();
        if (!p)
            return;
    }
    slot = SceneActorControl_AllocateSlot(p);
    if (!slot)
        return;
    slot->kind = 1;
    if (Script_SeekToKeyword(0x70)) {
        slot->savedPosition.x = Script_GetPc() ? Script_GetValue() : 0;
        slot->savedPosition.y = Script_GetPc() ? Script_GetValue() : 0;
        slot->savedPosition.z = Script_GetPc() ? Script_GetValue() : 0;
    } else {
        slot->savedPosition.x = 0;
        slot->savedPosition.y = 0;
        slot->savedPosition.z = 0;
    }
    flip = Script_GetKeywordValue(0x72, 0);
    slot->scene = Region_FindContaining(&slot->savedPosition);
    {
        u16 *temp = &slot->flags;
        slotFlags = temp;
    }
    {
        u32 zero = 0;
        *slotFlags = zero;
        slot->parameter = zero;
    }
    sprite = (void *)slot;
    slot->position = slot->savedPosition;
    Sprite_InitUnlinked(slot, p->graphics, 0);
    Anim_SetSequence(&slot->sequence, (void *)p->sequenceResource, 1, 0, flip);
    {
        register u16 *lateFlags asm("r6");
        lateFlags = Step(slot, sprite, slotFlags);
        {
            u32 value;
            {
                register struct SceneActorControlSprite *read asm("r1") = sprite;
                value = read->flags;
            }
            value &= ~1;
            {
                struct SceneActorControlSlot *write = (void *)sprite;
                write->spriteFlags = value;
                write->priority = 1;
            }
        }
        if (AreaEffect_HasEnabledTerrainValue(slot->scene)) {
            u32 value = *lateFlags;
            u32 mask = 0x20;
            mask |= value;
            *lateFlags = mask;
        }
    }
    SceneActorControl_InitCollider(slot, 0, 0);
}
