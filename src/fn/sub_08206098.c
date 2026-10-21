// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "display_animation.h"
#include "scene_vec.h"

struct OozeInit {
    u8 pad0[0x18];
    struct SceneVec initialPosition;
    struct DisplaySequenceSprite sprite;
    u8 animationData[0x1c];
    u8 collider[0x54];
    u8 body[0x48];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s16 extentX, extentY, extentZ, timer;
    s16 interval, variant;
    u8 pad124[0x14];
    s16 repeatAttack, repeatLimit, repeatCount, attackDelay, phase, unknown142;
    s16 facing;
    u16 idleSequence, startSequence, holdSequence, endSequence;
    s16 solid;
    void *parent;
    u8 pad154[4];
    void *resource;
    void *delayScript;
};
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
s32 TerrainOverlay_QueueAddActor(void *);
u32 Script_GetValueSafe(void);
void Ooze_InitAttackCollider(void *);
void Ooze_ReadScriptValueList(void *);
void Ooze_UpdateAttackDelay(void);
static inline u32 Ooze_StepInitialAnimation(struct DisplaySequenceSprite *sprite,
                                            struct DisplaySequenceState *state)
{
    const volatile u16 *command; /* preserve observed packed-command reads */
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
    return ended;
}

static inline void CopyPosition(struct SceneVec *out, const struct SceneVec *in)
{
    *out = *in;
}
static inline void Ooze_OffsetBodyPosition(struct SceneVec *position, s32 facing)
{
    if (!facing)
        position->x += 128;
    else
        position->z += 128;
}
s32 Ooze_InitializeFromScript(struct OozeInit *p, void *parent)
{
    struct SceneVec position;
    register s32 result asm("r0");
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        p->extentX = Script_GetValue();
        p->extentY = Script_GetValue();
        p->extentZ = Script_GetValue();
    } else {
        p->extentX = 30;
        p->extentY = 10;
        p->extentZ = 30;
    }
    if (Script_SeekToKeyword('i'))
        p->interval = Script_GetValue();
    else
        p->interval = 60;
    if (Script_SeekToKeyword('t'))
        p->variant = Script_GetValue();
    if (Script_SeekToKeyword('r'))
        p->delayScript = (void *)Script_GetValueSafe();
    if (Script_SeekToKeyword('w')) {
        p->attackDelay = Script_GetValue();
        p->repeatLimit = Script_GetValue();
        if (p->repeatLimit == 0)
            p->repeatLimit = 1;
        if (p->repeatLimit == 1)
            p->repeatAttack = 0;
        else
            p->repeatAttack = 1;
    } else {
        p->attackDelay = 30;
        p->repeatAttack = 1;
        p->repeatLimit = 3;
    }
    if (Script_SeekToKeyword('h')) {
        p->facing = Script_GetValue();
        {
            u32 facing = (u16)(p->facing = (p->facing - 1) / 2);
            asm("" : "+r"(facing));
            if (facing <= 3) {
                asm("" : "+r"(facing));
                switch (facing) {
                    case 0:
                    case 1:
                        p->facing = 0;
                        break;
                    case 2:
                    case 3:
                        p->facing = 1;
                        break;
                    default:
                        p->facing = 1;
                        break;
                }
            } else
                p->facing = 1;
        }
    } else
        p->facing = 0;
    if (Script_SeekToKeyword('S'))
        p->solid = Script_GetValue();
    p->parent = parent;
    AnimData_Load(p->animationData, 0x7e7d);
    Sprite_Init(&p->sprite, p->animationData, 0x201);
    *(struct SceneVec *)((u8 *)p + 0x3c) = p->initialPosition;
    AnimData_SelectPaletteBank(p->animationData, 0x1c3);
    p->resource = ScriptProc_Start(0x922e, 0x5184);
    Anim_SetSequence(&p->sequence, p->resource, 0x22, (u8)p->facing, 8);
    Ooze_StepInitialAnimation(&p->sprite, &p->sequence);
    {
        u32 off = 0x122;
        asm("" : "+r"(off));
        if (*(s16 *)((u8 *)p + off) == 0) {
            off += 0x24;
            asm("" : "+r"(off));
            *(u16 *)((u8 *)p + off) = 0x22;
            p->startSequence = 0x23;
            off += 4;
            asm("" : "+r"(off));
            *(u16 *)((u8 *)p + off) = 0x24;
            p->endSequence = 0x25;
        } else {
            off = 0x146;
            asm("" : "+r"(off));
            *(u16 *)((u8 *)p + off) = 0x1e;
            p->startSequence = 0x1f;
            off += 4;
            asm("" : "+r"(off));
            *(u16 *)((u8 *)p + off) = 0x20;
            p->endSequence = 0x21;
        }
        asm("" ::"r"(off));
    }
    Ooze_InitAttackCollider(p);
    {
        register u32 d asm("r0");
        asm("" : "=r"(d));
        if (p->delayScript)
            Ooze_ReadScriptValueList(p);
        asm("" ::"r"(d));
    }
    {
        register u32 d0 asm("r0");
        register s32 r asm("r1");
        asm volatile("" : "=r"(d0));
        r = p->solid;
        asm volatile("" ::"r"(d0));
        if (r) {
            CopyPosition(&position, (struct SceneVec *)((u8 *)p + 0x3c));
            if (p->variant == 1) {
                Ooze_OffsetBodyPosition(&position, p->facing);
            }
            SceneBody_Init(p->body, p->parent, &position, 0, 4, p);
            SceneBody_SetSprite(p->body, &p->sprite);
            result = TerrainOverlay_QueueAddActor(p->body);
        }
    }
    p->state = Ooze_UpdateAttackDelay;
    return result;
}
