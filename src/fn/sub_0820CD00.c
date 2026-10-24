// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: orientation range guard and later address/allocation ordering.
#include "global.h"
#define SCREAMER_TEMP     "r2"
#define SCREAMER_FIRST    "r2"
#define SCREAMER_MASK     "r7"
#define SCREAMER_DURATION "r4"
#define SCREAMER_WEIGHT   "r2"
#include "screamer_playback.h"
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *);
void SceneDrawablePair_SetFlags(void);
s32 Screamer_InitColliders(struct ScreamerState *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
inline static u32 Screamer_StepAnimation_(struct ScreamerState *owner)
{
    struct ScreamerSprite *sprite = &owner->sprite;
    struct DisplaySequenceState *state = &owner->sequence;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r2");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r7");
        register u32 temp asm("r2");
        register u32 savedDuration asm("r4");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
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
                command = (const volatile u16 *)(((u32)table) + offset);
                table = state->commands;
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r2") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!((u8)speed))
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

s32 Screamer_InitFromScript(struct ScreamerState *p, void *resource)
{
    struct ActorPosition position;
    void *graphics;
    void *sequences;
    s32 value;
    typeof(p->f1D8) t1;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        p->f1C4 = Script_GetValue();
        p->f1C6 = Script_GetValue();
        p->f1C8 = Script_GetValue();
    } else {
        p->f1C4 = 30;
        p->f1C6 = 10;
        p->f1C8 = 30;
    }
    if (Script_SeekToKeyword('l')) {
        value = Script_GetValue();
        p->f1E6 = value;
        p->f1E4 = value;
        if (((s16)value) < 0)
            p->f1F6 = 1;
    } else {
        p->f1E6 = 120;
        p->f1E4 = 120;
    }
    if (Script_SeekToKeyword('i'))
        p->f1CC = Script_GetValue();
    else
        p->f1CC = 120;
    if (Script_SeekToKeyword('r'))
        p->f1DA = Script_GetValue();
    else
        p->f1DA = 256;
    if (Script_SeekToKeyword('w')) {
        p->f1D4 = Script_GetValue();
        p->f1D0 = Script_GetValue();
    } else {
        p->f1D4 = 30;
        p->f1D0 = 3;
    }
    if (Script_SeekToKeyword('h')) {
        p->f1D8 = Script_GetValue();
        p->f1D8 = (t1 = (p->f1D8 - 1) / 2);
        switch ((u32)((u16)t1)) {
            case 0:

            case 1:
                p->f1D8 = 0;
                break;

            case 2:

            case 3:
                p->f1D8 = 1;
                break;

            default:
                p->f1D8 = 1;
                break;
        }

    } else
        p->f1D8 = 0;
    if (Script_SeekToKeyword('S'))
        p->f1EE = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->f1E8 = Script_GetValue();
        p->f1EA = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->f1EC = Script_GetValue();
    else
        p->f1EC = 2;
    if (Script_SeekToKeyword('D'))
        p->f214 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->f208 = Script_GetValue();
        p->f20C = Script_GetValue();
        p->f210 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->f1F0 = Script_GetValue();
        p->f1F2 = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->f1F4 = Script_GetValue();
        if (p->f1F4 > 99)
            p->f1F4 = 99;
        if (p->f1F4 <= 0)
            p->f1F4 = 1;
    } else
        p->f1F4 = 0;
    p->f1F8 = resource;
    graphics = p->graphics;
    AnimData_Load(graphics, 0x7E7D);
    Sprite_Init(&p->sprite, graphics, 1);
    p->sprite.position = p->initialPosition;
    AnimData_SelectPaletteBank(graphics, 0x1C4);
    *(((u8 *)(&p->sprite)) + 7) = 3;
    sequences = ScriptProc_Start(0x922E, 0x5184);
    p->f218 = sequences;
    if (!p->f1F6) {
        p->update = Screamer_WaitForPlayerApproach;
        Anim_SetSequence(&p->sequence, sequences, 0x1A, (u8)p->f1D8, 8);
    } else {
        p->update = SceneDrawablePair_SetFlags;
        p->f1E0 = 1;
        Anim_SetSequence(&p->sequence, p->f218, 0x1A, (u8)p->f1D8, 8);
    }
    Screamer_StepAnimation_(p);
    if (!p->f1F6)
        Screamer_InitColliders(p);
    if (!p->f1D8)
        p->sprite.position.x -= 16;
    else
        p->sprite.position.z -= 16;
    if (p->f1EE) {
        struct ActorPosition *offsetPosition;
        position = p->sprite.position;
        offsetPosition = &position;
        if (!p->f1D8)
            offsetPosition->x += 128;
        else
            offsetPosition->z += 128;
        SceneBody_Init(p->body, p->f1F8, &position, 0, 4, p);
        SceneBody_SetSprite(p->body, &p->sprite);
        TerrainOverlay_QueueAddActor(p->body);
    }
    if (p->f1DA > 0)
        p->f1FC = p->f1DA * p->f1DA;
    else
        p->f1FC = p->f1DA;
}

#else
INCLUDE_ASM("asm/nonmatching", Screamer_InitFromScript);
#endif
