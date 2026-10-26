// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
// confidence=low: creates a pooled projectile with optional spin and collider.
#define SpriteProjectile_Create sub_08127590
#ifdef NONMATCHING
// fuzzy: 84.9%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: guard evaluation order, animation temporaries and register allocation.
struct ParticleVector {
    u16 x, y, z, unused;
};
struct ParticleSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Projectile {
    u32 flags;
    u8 unknown4[2], angle, unknown7[9];
    u16 frame;
    u8 unknown12[10];
    struct ParticleVector position;
    u8 unknown24[8];
    struct ParticleSequence sequence;
    u8 unknown3C[0x1C];
    u8 collider[0x58];
    struct ParticleVector velocity;
    u8 unknownB8;
    s8 spin;
    u8 spinScale, colliderFlags;
    u16 timer, state;
    u8 active;
    s8 slot;
    u8 unknownC2[2];
    u32 resourceId;
    u8 enter, soundDelay;
    u16 lifetime, cell;
};
struct ProjectilePool {
    u8 unknown0[0x860];
    u32 allocated;
};
static inline u32 Step(struct Projectile *owner, struct ParticleSequence *state,
                       struct Projectile *base)
{
    struct Projectile *sprite = owner;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    base->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            base->flags |= 4;
        else
            base->flags &= ~4;
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

void *SpriteProjectile_EnsurePool(void);
struct Projectile *SpritePool10_AllocateSlot(void *);
extern u32 gEngineFlags, gUnk_030053F0, gUnk_02000560;
extern u32 gRandomIndex;
extern const u16 gUnk_0203B400[];
void AnimData_Load(void *, u32);
void *ScriptProc_Start(u32, u16);
void Sprite_Init(void *, void *, s32);
void Anim_SetSequence(void *, void *, u16, u8, u8);
void AnimData_SelectPaletteBank(u32, s32);
void sub_081274F4(void *, s32, s32, s32, s32);
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return gUnk_0203B400[gRandomIndex];
}
static inline u32 Busy(u32 mask)
{
    return (gEngineFlags | gUnk_030053F0) & mask;
}
static inline u32 TestFlags(u32 *value, u32 mask)
{
    return *value & mask;
}
inline static u32 Step_1(struct Projectile *sprite, struct Projectile *owner,
                         struct ParticleSequence *state, struct Projectile *base)
{
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = base->sequence.commands;
        command = (const volatile u16 *)(((u32)table) + offset);
    }
    base->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            base->flags |= 4;
        else
            base->flags &= ~4;
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
        if (first != ((((*command) & 0x30) >> 4) & bit))
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
                command = (const volatile u16 *)(((u32)table) + offset);
            }
            state->frameWeight = (*command) & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
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

struct Projectile *SpriteProjectile_Create(struct ParticleVector *position0,
                                           struct ParticleVector *velocity, u32 resource0,
                                           u32 script0, u32 sequence, u32 lifetime, u32 flags,
                                           u32 colliderFlags, s32 palette, u32 flip, u32 speed,
                                           s32 colliderKind, s32 sizeX, s32 sizeY, s32 sizeZ)
{
    register struct ParticleVector *position asm("sb") = position0;
    register u32 resource asm("r8") = resource0;
    register u32 script asm("sl") = script0;
    register struct ProjectilePool *pool asm("r3") = gPooledSpriteManager;
    struct Projectile *p;
    if (!pool) {
        pool = SpriteProjectile_EnsurePool();
        if (!pool)
            return 0;
    }
    if (Busy(0x400))
        return 0;
    if (TestFlags(&gUnk_02000560, 4))
        return 0;
    p = SpritePool10_AllocateSlot(pool);
    if (!p)
        return 0;
    p->state = 0;
    p->enter = 1;
    p->velocity = *velocity;
    p->lifetime = lifetime;
    p->colliderFlags = colliderFlags;
    p->resourceId = resource;
    p->soundDelay = 0;
    if (!p->colliderFlags) {
        p->active = 1;
        asm("" : : : "memory");
    } else {
        p->active = 2;
        asm("" : : : "memory");
    }
    {
        register struct ParticleVector *src asm("r2") = position;
        asm("" : "+r"(src));
        p->position = *src;
    }
    AnimData_Load(((u8 *)p) + 0x3C, resource);
    *((void **)(((u8 *)p) + 0xAC)) = ScriptProc_Start(0x922E, script);
    Sprite_Init(p, ((u8 *)p) + 0x3C, 0);
    Anim_SetSequence(&p->sequence, *((void **)(((u8 *)p) + 0xAC)), sequence, flip, speed);
    AnimData_SelectPaletteBank(*((u32 *)(((u8 *)p) + 0xC)), palette);
    Step_1(p, p, &p->sequence, p);
    p->flags |= 1 | flags;
    ((u8 *)p)[7] = 1;
    if (flags & 2) {
        do {
            p->angle = Rand();
        } while (0);
        if (((u8)Rand()) <= 127)
            p->spin = -1;
        else
            p->spin = 1;
        p->spinScale = (Rand() & 7) + 7;
    }
    if (colliderKind > 0) {
        sub_081274F4(p, colliderKind, sizeX, sizeY, sizeZ);
        p->colliderFlags |= 1;
    }
    return p;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08127590);
#endif
