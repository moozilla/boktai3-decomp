// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0820843C(void);
void sub_082086B8(void);
void CannonTurret_InitCollider(void *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
#define H(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline u32 StepAnimation(u8 *owner, struct DisplaySequenceState *stateArg, u32 flipMask)
{
    register struct DisplaySequenceSprite *sprite asm("ip") =
        (struct DisplaySequenceSprite *)(owner + 0x24);
    struct DisplaySequenceState *state = stateArg;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    {
        u32 frame = *command >> 6;
        struct DisplaySequenceSprite *o = (struct DisplaySequenceSprite *)(owner + 0x24);
        o->frame = frame;
    }
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit)) {
            u32 flags = (*(u32 *)(owner + 0x24));
            u32 flip = flipMask;
            (*(u32 *)(owner + 0x24)) = flags | flip;
        } else
            (*(u32 *)(owner + 0x24)) &= ~4;
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
        {
            register u32 flags asm("r1");
            if (first != (((*command & 0x30) >> 4) & bit)) {
                register struct DisplaySequenceSprite *read asm("r2") = sprite;
                flags = read->flags | 8;
            } else {
                register struct DisplaySequenceSprite *read asm("r2") = sprite;
                flags = read->flags & ~8;
            }
            {
                register struct DisplaySequenceSprite *out asm("r3") = sprite;
                out->flags = flags;
            }
        }
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r4");
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
                register s32 weight asm("r2") = state->frameWeight;
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

s32 CannonTurret_InitializeFromScript(u8 *p, void *parent)
{
    void *resource;
    u32 sequenceResult;
    if (Script_SeekToKeyword('p')) {
        H(0x1c) = Script_GetValue();
        H(0x1e) = Script_GetValue();
        H(0x20) = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 r = Script_GetValue();
        W(0x11c) = r * r;
    } else
        W(0x11c) = 0x40000;
    if (Script_SeekToKeyword('a')) {
        H(0x120) = Script_GetValue();
        H(0x122) = Script_GetValue();
        H(0x124) = Script_GetValue();
    } else {
        H(0x120) = 30;
        H(0x122) = 0;
        H(0x124) = 30;
    }
    if (Script_SeekToKeyword('l')) {
        {
            s32 health = Script_GetValue();
            H(0x136) = health;
            H(0x132) = health;
            if ((s16)health < 0)
                H(0x146) = 1;
        }
    } else {
        H(0x136) = 120;
        H(0x132) = 120;
    }
    if (Script_SeekToKeyword('s'))
        H(0x12a) = Script_GetValue();
    else
        H(0x12a) = 12;
    if (Script_SeekToKeyword('S'))
        H(0x13e) = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        H(0x138) = Script_GetValue();
        H(0x13a) = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        H(0x13c) = Script_GetValue();
    else
        H(0x13c) = 2;
    if (Script_SeekToKeyword('D'))
        W(0x160) = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        W(0x154) = Script_GetValue();
        W(0x158) = Script_GetValue();
        W(0x15c) = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        H(0x140) = Script_GetValue();
        H(0x142) = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        H(0x144) = Script_GetValue();
        if (H(0x144) > 99)
            H(0x144) = 99;
        if (H(0x144) <= 0)
            H(0x144) = 1;
    } else
        H(0x144) = 0;
    *(void **)(p + 0x148) = parent;
    AnimData_Load(p + 0x50, 0x7e7d);
    Sprite_Init(p + 0x24, p + 0x50, 1);
    *(struct SceneVec *)(p + 0x40) = *(struct SceneVec *)(p + 0x1c);
    AnimData_SelectPaletteBank(p + 0x50, 0x1bc);
    resource = ScriptProc_Start(0x922e, 0x5184);
    *(void **)(p + 0x164) = resource;
    if (H(0x146) == 0) {
        {
            u32 off = 0x108;
            u8 *q;
            q = p + off;
            *(void (**)(void))q = sub_0820843C;
        }
        sequenceResult = Anim_SetSequence(p + 0x10c, resource, 8, 0, 0);
    } else {
        *(void (**)(void))(p + 0x108) = sub_082086B8;
        {
            u32 zero;
            s16 *q = &H(0x130);
            zero = 0;
            *q = 1;
            asm("" : : : "memory");
            sequenceResult = Anim_SetSequence(p + 0x10c, *(void **)(p + 0x164), 10, 0, zero);
        }
    }
    StepAnimation(p, (void *)(p + 0x10c), 4);
    {
        register u32 off0 asm("r3") = 0x146;
        register u8 *a0 asm("r1");
        asm("" : "+r"(off0));
        a0 = p + off0;
        if (*(s16 *)a0 == 0)
            CannonTurret_InitCollider(p);
    }
    {
        u32 off = 0x13e;
        if (*(s16 *)(p + off) != 0) {
            u8 *body = p + 0xc0;
            void *par;
            u8 *pa;
            off += 0xa;
            pa = p + off;
            par = *(void **)pa;
            SceneBody_Init(body, par, p + 0x40, 0, 4, p);
            SceneBody_SetSprite(body, p + 0x24);
            TerrainOverlay_QueueAddActor(body);
        }
    }
}
