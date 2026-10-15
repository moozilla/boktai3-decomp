// MATCHING_CONCESSION: register-pinning
#include "scene_panel_subject_tail.h"
/* Preserve the decoder's separate frame and flip-bit command reads. */
static inline u32 ScenePanelSubject_StepInitialPlayback(struct ScenePanelSubjectTail *owner)
{
    struct ScenePanelSubjectDrawable *sprite = &owner->drawable;
    struct DisplaySequenceState *state = &owner->sequence;
    const volatile u16 *command;
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
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
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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
    return ended;
}
void sub_08232264(void *, void *, u32, u32, u32);
#define ScenePanelSubject_Initialize sub_0805D92C
s32 ScenePanelSubject_Initialize(struct ScenePanelSubjectTail *subject,
                                 const struct ContactPosition *home, u32 row, u32 variant,
                                 u32 initialState, u32 kind, u32 contactLimit, u32 colliderSetting,
                                 u32 appearDelay)
{
    struct ScenePanelSubjectDrawable *sprite;
    subject->home = *home;
    subject->kind = kind;
    subject->row = row;
    subject->variant = variant;
    subject->appearDelay = appearDelay;
    subject->contactLimit = contactLimit;
    subject->progressStep = Div(subject->contactLimit, 6);
    sprite = &subject->drawable;
    AnimData_Load(subject->graphics, 0xC038);
    Sprite_Init(sprite, subject->graphics, 0);
    sprite->position = subject->home;
    subject->resource = ScriptProc_Start(0x922E, 0x3449);
    sub_0805D8A8(subject, colliderSetting);
    sub_0805CB88((void *)subject, initialState);
    switch (subject->state) {
        case 0:
            subject->contactProgress = 0;
            subject->tile = 0x24C;
            subject->drawable.priority = 3;
            subject->drawable.flags |= 1;
            break;
        case 2:
            subject->contactProgress = 0;
            subject->tile = 0x24C;
            Anim_SetSequence(&subject->sequence, subject->resource, 1, subject->row,
                             subject->variant);
            ScenePanelSubject_StepInitialPlayback(subject);
            Collider_Refresh(&subject->collider);
            break;
        case 4: {
            register const u16 *limit asm("r2") = &subject->contactLimit;
            asm("" : : "r"(limit));
            subject->contactProgress = *limit;
        }
            subject->tile = 0x24D;
            Anim_SetSequence(&subject->sequence, subject->resource, 3, subject->row,
                             subject->variant);
            ScenePanelSubject_StepInitialPlayback(subject);
            Collider_Refresh(&subject->collider);
            break;
    }
    AnimData_SelectTiles(subject->graphics, subject->tile);
    subject->timer = 0;
    subject->changed = 0;
    sub_08232264(subject->effects, &subject->drawable.position, 0, 0, 0);
    subject->unknownEE = 0;
    return 0;
}
