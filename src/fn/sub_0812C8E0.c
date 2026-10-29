// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "ram.h"
struct FragmentSprite {
    u32 flags;
    u8 active, unknown5[2], priority;
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
};
struct Fragment {
    u32 flags;
    u8 active, unk_05[2], priority;
    s8 scaleX, scaleY;
    u8 unk_0A[2];
    void *graphics;
    u16 frame;
    u8 unk_12[10];
    struct ContactPosition position;
    u8 unk_24[8];
    struct DisplaySequenceState sequence;
    u8 collider[0x54];
    struct ContactPosition center, offset;
    u32 angle, angularSpeed;
    s16 direction;
    u16 unk_AA;
    s16 radius, scaleTimer;
    void *resource;
    s32 lifetime;
    u16 unk_B8;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered;
};
struct Pool {
    u8 unk_00[0x18];
    u8 graphics[0x20];
    u32 used;
};
s32 Mod(s32, s32);
s32 Div(s32, s32);
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);
void Sprite_Remove(void *);
static inline void FragmentEffect_StepAnimation(struct Fragment *sprite, u32 initializing)
{
    struct DisplaySequenceState *state = &sprite->sequence;
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
        bit = 1;
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
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        /* Preserve the mask allocation at the two animation call sites. */
        if (initializing) {
            register u32 source asm("r2") = 0xffff;
            register u32 fixedMask asm("r4");
            asm("" : : "r"(source));
            fixedMask = source;
            asm("" : "+&r"(fixedMask) : "r"(0xffff));
            mask = fixedMask;
        } else {
            register u32 source asm("r1") = 0xffff;
            register u32 fixedMask asm("r6");
            asm("" : : "r"(source));
            fixedMask = source;
            asm("" : "+&r"(fixedMask) : "r"(0xffff));
            mask = fixedMask;
        }
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
// Tentative: shrink the fragment, then release its sprite and pool slot.
void FragmentEffect_ShrinkAndRemove(struct Pool *pool, struct Fragment *p)
{
    s16 *scaleTimer;
    u16 *timer;
    if (p->entered) {
        p->flags = (p->flags & ~1) | 2;
        p->scaleTimer = 20;
        p->timer = 0;
        p->entered = 0;
        p->scaleX = 64;
        p->scaleY = 64;
        p->timer = 0;
    }
    {
        u16 *frame = &p->timer;
        s32 remainder = Mod(*frame, 6);
        timer = frame;
        if (!remainder)
            FragmentEffect_SpawnRandomParticle(&p->position);
    }
    {
        register s16 *initial asm("r0") = &p->scaleTimer;
        s32 remaining = *initial;
        scaleTimer = initial;
        if (remaining > 0) {
            p->scaleX += Div(1 - p->scaleX, *scaleTimer);
            p->scaleY += Div(1 - p->scaleY, *scaleTimer);
            (*scaleTimer)--;
        }
    }
    FragmentEffect_StepAnimation(p, 0);
    if (*scaleTimer <= 0) {
        Sprite_Remove(p);
        pool->used &= ~(1 << p->slot);
        p->slot = -1;
        p->state = 0;
        p->entered = 0;
    } else {
        (*timer)++;
    }
}
struct Pool *FragmentEffect_GetOrCreateManager(void);
struct Fragment *sub_0812C3D0(struct Pool *);
void FragmentEffect_InitCollider(struct Fragment *, u32, u32, u32, u32, u32, u32, u32);
extern const s16 gUnk_086149C4[];
static inline s32 FragmentEffect_Truncate(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void FragmentEffect_ComputeOffset(struct ContactPosition *out, u32 heading,
                                                s32 radius)
{
    u32 angle = heading >> 12;
    s32 scale = radius * 128;
    out->x = FragmentEffect_Truncate(scale * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = FragmentEffect_Truncate(scale * gUnk_086149C4[angle & 255]);
}
#define FragmentEffect_Create sub_0812CAAC
void FragmentEffect_Create(const struct ContactPosition *position, u32 angle, u32 angularSpeed,
                           s32 radius, s32 direction, s32 lifetime, u32 a, u32 b, u32 c, u32 d)
{
    struct Pool *pool = (struct Pool *)gFragmentEffectManager;
    struct Fragment *p;
    struct FragmentSprite *sprite;
    struct ContactPosition offset;
    if (!pool) {
        pool = FragmentEffect_GetOrCreateManager();
        if (!pool)
            return;
    }
    p = sub_0812C3D0(pool);
    if (!p)
        return;
    p->state = 1;
    p->lifetime = lifetime;
    p->unk_B8 = 0;
    p->angle = angle << 12;
    p->angularSpeed = angularSpeed;
    p->radius = radius;
    p->direction = direction;
    p->entered = 1;
    p->center = *position;
    sprite = (struct FragmentSprite *)p;
    sprite->position = p->center;
    {
        u32 heading = p->angle;
        FragmentEffect_ComputeOffset(&offset, heading, p->radius);
    }
    sprite->position.x += offset.x;
    sprite->position.z += offset.z;
    p->resource = ScriptProc_Start(0x922e, 0x121b);
    Sprite_Init(sprite, pool->graphics, 0);
    Anim_SetSequence(&p->sequence, p->resource, 7, 0, 0);
    AnimData_SelectPaletteBank(sprite->graphics, 0x127);
    FragmentEffect_StepAnimation(p, 1);
    sprite->priority = 2;
    sprite->flags &= ~1;
    FragmentEffect_InitCollider(p, a, b, c, d, 0, 0, 0);
}
