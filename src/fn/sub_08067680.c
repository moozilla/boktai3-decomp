// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control.h"
#include "ram.h"
struct SceneActorControlManager *SceneActorControl_EnsureSingleton(void);
struct SceneActorControlSlot *SceneActorControl_AllocateSlot(struct SceneActorControlManager *);
void SceneActorControl_DecodeDirection(const void *, u8 *, u8 *);
s32 Region_FindContaining(const struct ContactPosition *);
void Sprite_InitUnlinked(void *, void *, u32);
u32 AreaEffect_HasEnabledTerrainValue(s32);
void SceneActorControl_InitCollider(struct SceneActorControlSlot *, s32, s32);
static inline u16 *Step(struct SceneActorControlSlot *slot,
                        struct SceneActorControlSprite *inputSprite, u16 *inputFlags)
{
    register struct SceneActorControlSprite *sprite asm("r10") = inputSprite;
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

        first = bit;
        first &= input;
        {
            u32 bits = *command;
            u32 flip = 0x30;
            flip &= bits;
            flip >>= 4;
            bits = 1;
            flip &= bits;
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
            register struct SceneActorControlSprite *write asm("r2");
            value |= 8;
            write = sprite;
            write->flags = value;
        } else {
            register struct SceneActorControlSprite *read asm("r1") = sprite;
            u32 value = read->flags;
            register struct SceneActorControlSprite *write asm("r2");
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
static inline u8 *PreserveKind(u8 *flip, u32 value)
{
    u32 one = value;
    return flip;
}
s32 SceneActorControl_CreateFromPosition(const struct SceneActorControlInput *argument)
{
    const struct SceneActorControlInput *input = argument;
    register struct SceneActorControlManager *found asm("r0") = gSceneActorControlManager;
    register struct SceneActorControlManager *p asm("r6");
    struct SceneActorControlSlot *slot;
    struct ContactPosition *saved;
    u8 unused, sequence;
    register struct SceneActorControlSprite *sprite asm("r10");
    u16 *slotFlags;
    if (!found) {
        p = SceneActorControl_EnsureSingleton();
        if (!p) {
        failure:
            return -1;
        }
    } else
        p = found;
    slot = SceneActorControl_AllocateSlot(p);
    if (!slot)
        goto failure;
    {
        register u32 zero asm("r5") = 0;
        u32 one = 1;
        slot->kind = one;
        SceneActorControl_DecodeDirection(input, &unused, PreserveKind(&sequence, one));
        saved = &slot->savedPosition;
        {
            const struct SceneActorControlInput *read = input;
            *saved = read->position;
        }
        slot->scene = Region_FindContaining(saved);
        {
            u16 *temp = &slot->flags;
            slotFlags = temp;
        }
        *slotFlags = zero;
        slot->parameter = zero;
    }
    sprite = (void *)slot;
    slot->position = *saved;
    Sprite_InitUnlinked(slot, (u8 *)p + 0x18, 0);
    {
        void *resource;
        u8 *read;
        resource = *(void **)((u8 *)p + 0x34);
        asm("" : "+r"(resource) : "r"(&slot->sequence));
        asm("" : "=r"(read) : "0"(&sequence), "r"(resource), "r"(&slot->sequence));
        Anim_SetSequence(&slot->sequence, resource, 1, 0, *read);
    }
    {
        u16 *lateFlags;
        lateFlags = Step(slot, sprite, slotFlags);
        {
            struct SceneActorControlSlot *write = (void *)sprite;
            u32 flags = write->spriteFlags;
            flags &= ~1;
            write->spriteFlags = flags;
            write->priority = 1;
        }
        if (AreaEffect_HasEnabledTerrainValue(slot->scene)) {
            u32 value = *lateFlags;
            u32 mask = 0x20;
            mask |= value;
            *lateFlags = mask;
        }
    }
    SceneActorControl_InitCollider(slot, 0, 0);
    return slot->index;
}
