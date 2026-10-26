// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "pink_spine_playback.h"
/* Species identity is tentative. The update callback increments age after animation. */
extern u32 gUnk_030053F0, gUnk_02000560;
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void ProjectileEmitter_EmitAttackFrame(void);
void ProjectileEmitter_UpdateAttack(void);
static inline u32 PauseFlags(u32 mask)
{
    return (gEngineFlags | gUnk_030053F0) & mask;
}
static inline u32 TestFlags(u32 bits)
{
    register u32 mask asm("r1") = bits;
    return gUnk_02000560 & mask;
}
static inline u32 PinkSpine_AdvanceDamageSequence(struct PinkSpineState *owner)
{
    struct ScreamerSprite *sprite = &owner->sprite;
    struct DisplaySequenceState *state = &owner->sequence;
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
        register u32 first asm("r2");
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
        register u32 first asm("r2");
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
        register u32 mask asm("r7");
        register u32 temp asm("r2");
        register u32 savedDuration asm("r4");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Preserve the observed constant-to-mask copy. */

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

s32 PinkSpine_UpdateDamageAnimation(struct PinkSpineState *p)
{
    s32 pauseMask;
    pauseMask = 4;
    if (PauseFlags(0x400) || TestFlags(pauseMask))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (!p->f134) {
        struct ColliderView {
            u8 pad[6];
            u16 flags;
        };
        register struct ColliderView *c asm("r2") = (void *)(&p->collider);
        {
            register u32 value = c->flags;
            u32 mask = 0xFFFB;
            mask &= value;
            c->flags = mask;
        }
        Collider_SetPosition(c, &p->sprite.position, 0);
    } else {
        struct ColliderView {
            u8 pad[6];
            u16 flags;
        } *c = (void *)(&p->collider);
        c->flags |= 4;
    }
    if ((p->f130 > 0) && (!p->f134)) {
        p->f136 -= p->f130;
        p->f144 = p->f148;
        Sound_PlaySong(0x13E);
        if (p->f136 <= 0) {
            p->f134 = 1;
            Anim_RestartSequence(&p->sequence, p->f15C, 0xD, 0, 0);
            AnimData_SelectPaletteBank(p->graphics, 0x1BD);
            p->f12C = 0;
            p->update = ProjectileEmitter_EmitAttackFrame;
            Sound_PlaySong(0x522);
            p->age = 0;
        }
        p->f12C = 4;
        p->f132 = 1;
        p->f130 = 0;
    }
    if (p->f12C > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->f12C--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1BD);
    {
        u32 ended;
        ended = PinkSpine_AdvanceDamageSequence(p);
        if (ended && (p->update == ProjectileEmitter_UpdateAttack))
            Sound_PlaySong(0x521);
    }
    p->age++;
}
