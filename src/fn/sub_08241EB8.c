// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "scene_vec.h"

struct PlayerSpiralPose {
    u8 unknown0[0x18];
    u32 song;
    u8 unknown1C[0x30 - 0x1C];
    struct SceneVec position;
    u8 unknown38[0xAC - 0x38];
    struct SceneVec savedPosition;
    u8 unknownB4[0xD4 - 0xB4];
    u32 animationResource;
    u8 unknownD8[0x14C - 0xD8];
    struct ContactSwarmSprite sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 facing, flip, dir;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState seq;
    u32 unknown40C;
    const void *resource;
    const void *resource2;
    u8 pad3[0x457 - 0x418];
    u8 tetherMode;
    u8 phase;
    u8 pad4;
    u16 timer;
    u8 unknown45C[8];
    u8 completion0, completion1;
    u8 pad5[0x4E8 - 0x466];
    u16 scale;
    u8 pad6[0x58C - 0x4EA];
    void (*fn)(struct PlayerSpiralPose *);
    u8 unknown590[0xAD6 - 0x590];
    u16 trailResource, trailWindow;
};
void Player_SetTetherMode(struct PlayerSpiralPose *, s32, s32);
void LinkPlayer_FlushPendingEffects(struct PlayerSpiralPose *);
void Player_PlayLocalSong(s32, u32);
s32 Player_SetSecondaryActionFlags(struct PlayerSpiralPose *, u32);
s32 Player_SetAnimationIndex(struct PlayerSpiralPose *, s32, s32);
void SpiralTrail_CreateWithParticles(struct PlayerSpiralPose *, s32, s32, s32, s32);
extern u8 *gUnk_030042E4;
static inline void Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration, flags, bit;
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
        register u32 mask asm("r5");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
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
                } else {
                    state->index = index - 1;
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
                } else {
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
        }
    }
}

// confidence=low: an alternate-form cue that counts up while sliding the pose window
void PlayerCue_UpdateSpiralPose(struct PlayerSpiralPose *p)
{
    if (p->tetherMode != 0xe) {
        Player_SetTetherMode(p, 0xe, 0);
        p->trailWindow = 0;
        LinkPlayer_FlushPendingEffects(p);
        Player_PlayLocalSong(p->song, 0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 0x20, 0x38);
    }
    {
        register u32 timerOffset asm("r3") = (u32) & ((struct PlayerSpiralPose *)0)->timer;
        u32 t;
        register s32 c asm("r1");
        register u32 low asm("r2");
        t = *(u16 *)((u8 *)p + timerOffset);
        asm("" : : "r"(timerOffset));
        c = 7;
        low = c;
        low &= t;
        if ((s32)low >= (c - (s32)(t >> 2))) {
            Player_SetSecondaryActionFlags(p, 0x41);
            p->savedPosition = p->position;
        }
    }
    Player_SetAnimationIndex(p, 0x9a, 0x40);
    p->animationResource = ((u32)gUnk_030042E4) + 0x2DE0;
    {
        register u32 offset asm("r3") = (u32) & ((struct PlayerSpiralPose *)0)->seq;
        struct DisplaySequenceState *seq;
        asm("" : "+r"(offset));
        seq = (struct DisplaySequenceState *)((u8 *)p + offset);
        Anim_SetSequence(seq, p->resource, 0, p->facing, p->flip);
        {
            register u32 spriteOffset asm("r3") = (u32) & ((struct PlayerSpiralPose *)0)->sprite;
            asm("" : "+r"(spriteOffset));
            Step((struct ContactSwarmSprite *)((u8 *)p + spriteOffset), seq);
        }
    }
    {
        register u32 offset asm("r2") = (u32) & ((struct PlayerSpiralPose *)0)->trailResource;
        register u16 *target asm("r1");
        u32 resource;
        asm("" : "+r"(offset));
        target = (u16 *)((u8 *)p + offset);
        resource = 0x16F;
        *target = resource;
    }
    {
        u32 window;
        register u32 mask asm("r2");
        register u32 temp asm("r3");
        register u32 windowOffset asm("r3") = (u32) & ((struct PlayerSpiralPose *)0)->trailWindow;
        u16 *windowField;
        asm("" : "+r"(windowOffset));
        windowField = (u16 *)((u8 *)p + windowOffset);
        window = *windowField + 3;
        *windowField = window;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        if ((u16)window > 0x20)
            *windowField = 0x20;
        {
            u32 nt = p->timer + 1;
            p->timer = nt;
            if ((nt & mask) > 0x1f) {
                p->completion0 = 1;
                p->completion1 = 1;
                Player_SetTetherMode(p, 0, 0);
            }
        }
    }
}
