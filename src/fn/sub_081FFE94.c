// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct HomingVec {
    s16 x, y, z, pad;
};
struct HomingParams {
    u8 unknown0[6];
    s8 angle;
    u8 variant;
    s32 speed;
    void *behavior;
    s16 attack, element, attackFlags, body;
    u8 unknown18[8];
};
struct HomingCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x4c];
};
struct HomingSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct HomingSprite {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xa];
    struct HomingVec position;
    u8 unknown24[8];
};
struct HomingSlot {
    u32 lifetime;
    u8 unknown04[8];
    struct HomingVec velocity;
    struct HomingVec origin;
    struct HomingVec target;
    struct HomingCollider secondaryCollider;
    struct HomingCollider primaryCollider;
    u8 animData[6];
    u16 paletteMode;
    u8 unknownD4[4];
    void *palette;
    u8 unknownDC[0xc];
    void *resource;
    struct HomingParams params;
    struct HomingSprite sprite;
    u32 unknown138;
    u32 state;
    struct HomingSequence sequence;
    u8 unknown150[4];
    u32 paletteModeCopy;
    u8 unknown158[4];
    s16 flashTimer;
    u8 unknown15E[2];
};
struct HomingManager {
    u8 task[0x18];
    u32 activeMask;
    struct HomingSlot slots[3];
    s32 count;
    u16 palettes[2][16];
};
extern void *const gUnk_08613FA8[];
extern const s16 gUnk_086149C4[];
void HomingProjectile_CreateManager(void);
struct HomingSlot *HomingProjectile_FindFreeSlot(struct HomingManager *, s32 *);
void HomingProjectile_SetBehaviorCallback(struct HomingParams *, void *);
void Memory_Copy(void *, const void *, s32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetPosition(void *, void *, u32);
s32 AnimData_Load(void *, s32);
void *ScriptProc_Start(u16, u16);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void Sprite_LinkToList(void *, u32);

static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct HomingVec *v, s32 angle, s32 inScale)
{
    register s32 scale asm("r6") = inScale;
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];
        v->z = Scale(sample * scale);
    }
}

static inline void StepSequence(struct HomingSprite *sprite, struct HomingSequence *state)
{
    const volatile u16 *command;
    u32 duration, flags, bit;
    command = state->commands + state->index;
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
                        state->index = frameCount - 1;
                    } else {
                        state->index = 0;
                    }
                }
            }
            command = state->commands + state->index;
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}

s32 HomingProjectile_Spawn(struct HomingParams *params, struct HomingVec *position)
{
    struct HomingManager *m = gHomingManager;
    struct HomingSlot *p;
    s32 index;
    u16 animId, resourceId;
    s32 sequence;
    u8 direction;
    u8 flips[2];
    u8 *first;
    u8 *second;
    u8 *saved;
    u32 check;
    if (!m) {
        HomingProjectile_CreateManager();
        return -1;
    }
    p = HomingProjectile_FindFreeSlot(m, &index);
    if (!p)
        return -1;
    HomingProjectile_SetBehaviorCallback(params, gUnk_08613FA8[params->variant]);
    Memory_Copy(&p->params, params, 0x20);
    p->lifetime = 0;
    p->state = 0;
    p->flashTimer = 0;
    Vector(&p->velocity, params->angle, params->speed);
    p->sprite.position = *position;
    p->target = *position;
    p->origin = *position;
    p->origin.y += 0x80;
    {
        struct HomingCollider *collider = &p->secondaryCollider;
        Collider_SetAttackProperties(collider, params->attack, params->element, 0, 0,
                                     params->attackFlags);
        Collider_SetPosition(collider, position, 0);
        collider++;
        Collider_SetBodyProperties(collider, params->body, 0x402, 1);
        Collider_SetPosition(collider, position, 0);
        {
            s32 mask = ~4;
            collider->flags = mask & collider->flags;
        }
    }
    if (p->params.variant == 0) {
        animId = 0x9247;
        resourceId = 0x1DF8;
        sequence = 7;
    } else {
        animId = 0x48DE;
        resourceId = 0xA4B2;
        sequence = 0;
    }
    AnimData_Load(p->animData, animId);
    p->palette = m->palettes[0];
    p->paletteModeCopy = p->paletteMode;
    p->resource = ScriptProc_Start(0x922E, resourceId);
    direction = (((u16)params->angle + 0x60) >> 6) & 3;
    first = &flips[0];
    second = &flips[1];
    check = (u8)(direction - 1);
    saved = second;
    if (check <= 1)
        *first = 1;
    else
        *first = 0;
    if (direction > 1)
        *second = 1;
    else
        *second = 0;
    Anim_SetSequence(&p->sequence, p->resource, (u16)sequence, flips[0], *saved);
    StepSequence(&p->sprite, &p->sequence);
    {
        register u32 offset asm("r2") = 0x10c;
        struct HomingSprite *sprite;
        asm("" : "+r"(offset));
        sprite = (void *)((u8 *)p + offset);
        sprite->flags &= ~1;
        Sprite_LinkToList(sprite, 0);
    }
    {
        register u32 offset asm("r3") = 0x113;
        register u8 *priority asm("r1");
        asm("" : "+r"(offset));
        priority = (u8 *)p + offset;
        *priority = 2;
    }
    {
        u32 bit = 1 << index;
        register struct HomingManager *held asm("r4") = m;
        held->activeMask |= bit;
    }
    m->count++;
    return index;
}
