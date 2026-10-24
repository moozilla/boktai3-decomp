// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: parameter flag operations and saved pointer lifetimes.

#include "contact_swarm.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
static inline u32 StepPlayback(struct Drawable *owner, struct DisplaySequenceState *state)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command;
    u32 ended;
    u32 bit;
    u32 duration;
    u32 flags;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        command = (const volatile u16 *)(((u32)table) + offset);
        table = state->commands;
    }
    sprite->frame = (*command) >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
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
        asm("" : "+r"(mask));
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

void Anim_RestartSequence(void *, void *, u16, u8, u8);
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_SetCollisionSize(void *, s32, s32, s32);
u32 sub_08249498(u32, u32);
void Task4E01_BeginImpactAnimation(void *);

void Task4E01_UpdateExpandingImpact(void *);
void Projectile_InitializeParams(void *);
void Projectile_SetFlightParams(void *, u32, u32, s32, s32);
void Projectile_SetDamageParams(void *, u32, u32, u32, u32, u32);
void Projectile_SetTypeAndElement(void *, u32, u32);
void Projectile_SetSpriteParams(void *, u32, u32, u32);
s32 Projectile_Spawn(void *, void *);
extern u32 gUnk_03005308;
struct SpawnConfig {
    u16 flags, lifetime;
    u8 pad4[2];
    s8 direction;
    u8 pad7;
    s32 speed;
    u8 padC[8];
    u32 damage[5];
    s32 sizeX, sizeZ;
    u8 pad30[12];
    u32 sprite[3];
    u32 type, element;
    u16 count;
};
// confidence=low: emits three randomized child projectiles before removing the source.
s32 Task4E01_SpawnImpactChildren(u8 *input)
{
    register u8 *p asm("r10") = input;
    u8 storage[0x58];
    u8 *params = storage;
    struct SpawnConfig *config = (void *)(p + 0x18);
    u32 direction = (u8)config->direction;
    register s32 speed asm("r6") = config->speed;
    register s32 i asm("r7");
    register u32 *randomIndex asm("r8");
    register const volatile u16 *randomTable asm("r9");
    if (!(config->flags & 0x10)) {
        randomIndex = &gUnk_03005308;
        i = 0;
        do {
            randomTable = gUnk_0203B400;
            if (config->flags & 0x20) {
                *(&gUnk_03005308) = ((*(&gUnk_03005308)) + 1) & 1023;
                {
                    u8 k1 = 32;
                    config->direction =
                        ((randomTable[*(&gUnk_03005308)] & 63) - k1) + ((u8)config->direction);
                }
            } else {
                *(&gUnk_03005308) = ((*(&gUnk_03005308)) + 1) & 1023;
                config->direction = randomTable[*(&gUnk_03005308)];
            }
            Projectile_InitializeParams(params);
            if (i == 0)
                *((u16 *)params) |= 0x110;
            else if (i == 1)
                *((u16 *)params) = ((*((u16 *)params)) & 0xfeef) | 0x210;
            else
                *((u16 *)params) = ((*((u16 *)params)) & 0xfdef) | 0x310;
            {
                s32 half;
                if (speed >= 0)
                    half = speed >> 1;
                else
                    half = -((-speed) >> 1);
                config->speed += half * (i - 1);
            }
            Projectile_SetFlightParams(params, *((u16 *)params), config->lifetime + 30,
                                       config->direction, config->speed);
            *((u16 *)params) |= config->flags;
            *((void **)(params + 0x54)) = config;
            Projectile_SetDamageParams(params, config->damage[0], config->damage[1],
                                       config->damage[2] >> 1, config->damage[3],
                                       config->damage[4]);
            Projectile_SetCollisionSize(storage, (s16)config->sizeX, 80, (s16)config->sizeZ);
            Projectile_SetTypeAndElement(storage, config->type, config->element);
            Projectile_SetSpriteParams(storage, config->sprite[0], config->sprite[1],
                                       config->sprite[2]);
            if (Projectile_Spawn(storage, p + 0xa8) < 0)
                config->count--;
            config->direction = direction;
            config->speed = speed;
            i++;
        } while (i <= 2);
    }
    StepPlayback((void *)(p + 0x8c), (void *)(p + 0x180));
    Task4E01_BeginImpactAnimation(p);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Task4E01_SpawnImpactChildren);
#endif
