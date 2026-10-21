// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.7%
// Remaining differences: outer switch layout, frame setup, and packed animation register allocation.

#include "contact_swarm.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
static inline u32 StepFirst(struct Drawable *owner, struct DisplaySequenceState *inputState)
{
    register struct DisplaySequenceState *state asm("r5") = inputState;
    register struct ContactSwarmSprite *sprite asm("r6") = &owner->sprite;
    register const volatile u16 *command asm("r7"); /* preserve observed packed-command reads */
    u32 ended, bit;
    register u32 duration asm("r2");
    register u32 flags asm("r1");
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
        register u32 maskBit asm("r4") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r4");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+r"(mask));
        /* Keep the live mask distinct from its source without binding r7. */

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

static inline u32 StepSecond(struct Drawable *owner, struct DisplaySequenceState *inputState)
{
    register struct DisplaySequenceState *state asm("r4") = inputState;
    register struct ContactSwarmSprite *sprite asm("r5") = &owner->sprite;
    register const volatile u16 *command asm("r6"); /* preserve observed packed-command reads */
    u32 ended, bit;
    register u32 duration asm("r2");
    register u32 flags asm("r1");
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
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r5");
        register u32 temp asm("r2");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+r"(mask));
        /* Keep the live mask distinct from its source without binding r7. */

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

void Anim_RestartSequence(void *, void *, u16, u8, u8);
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_SetCollisionSize(void *, s32, s32);
u32 sub_08249498(u32, u32);
void Task4E01_SpawnImpactChildren(void *);
void Task4E01_UpdateExpandingImpact(void *);
void Task4E01_OnDefaultDirectionalAnimation(void *);
void Task4E01_OnType7DirectionalAnimation(void *);
void Task4E01_OnType8DirectionalAnimation(void *);
void Task4E01_OnType4Animation(void *);
void Projectile_UpdateAnimation(void *);
void sub_081D9D3C(void *);
// confidence=low: resets projectile time, chooses impact animation, and sets the impact flag.
struct ImpactConfig {
    u16 flags;
    u8 pad[6];
    u32 time;
    u8 padC[0x48 - 12];
    u32 kind;
};
struct ImpactHeader {
    u32 time, flags;
    u8 pad[16];
    struct ImpactConfig config;
};
s32 Task4E01_BeginImpactAnimation(u8 *input)
{
    register u8 *p asm("r8") = input;
    register u8 *config asm("r9") = input + 0x18;
    ((struct ImpactHeader *)input)->config.flags |= 0x1000;
    ((struct ImpactHeader *)input)->time = 0;
    ((struct ImpactConfig *)config)->time = 0;
    switch (((struct ImpactConfig *)config)->kind) {
        case 0:
        case 1:
        case 2:
        case 5:
        case 6:
        case 10:
            ((struct Drawable *)(input + 0x8c))->sprite.frame = 29;
            Projectile_SetUpdateCallback(config, Task4E01_OnDefaultDirectionalAnimation);
            ((struct ImpactHeader *)input)->time = 0;
            break;
        case 7:
            ((struct Drawable *)(input + 0x8c))->sprite.frame = 29;
            Projectile_SetUpdateCallback(config, Task4E01_OnType7DirectionalAnimation);
            break;
        case 8:
            ((struct Drawable *)(input + 0x8c))->sprite.frame = 29;
            Projectile_SetUpdateCallback(config, Task4E01_OnType8DirectionalAnimation);
            break;
        case 4:
            Projectile_SetUpdateCallback(config, Task4E01_OnType4Animation);
            break;
        case 3:
            if (*(u16 *)config & 0x10) {
                *(u32 *)(input + 0x194) = 1;
                Anim_RestartSequence(input + 0x180, *(void **)(input + 0x19c), 1, 0, 0);
                StepFirst((void *)(input + 0x8c), (void *)(input + 0x180));
            } else {
                *(u32 *)(input + 0x194) = 6;
                Anim_RestartSequence(input + 0x180, *(void **)(input + 0x19c), 6, 0, 0);
                StepSecond((void *)(input + 0x8c), (void *)(input + 0x180));
            }
            Projectile_SetUpdateCallback(config, Projectile_UpdateAnimation);
            break;
        case 11:
            if (!(*(u16 *)config & 0x2000))
                ((struct Drawable *)(input + 0x8c))->sprite.frame = 31;
            Projectile_SetUpdateCallback(config, sub_081D9D3C);
            break;
        case 9:
        default:
            break;
    }
    ((struct ImpactHeader *)input)->flags |= 1;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Task4E01_BeginImpactAnimation);
#endif
