#include "global.h"
#include "engine_globals.h"
#include "ram.h"

// confidence=low: updates homing effect entries, animation, trails and lifetime.

struct Types {
    u32 values[4];
};
struct Vec {
    s16 x, y, z, pad;
};
struct Animation {
    const u16 *data;
    u8 flags, count, duration, rate;
    u16 frame, unknownA, scale, timer;
};
struct Entry {
    u32 flags;
    u8 pad4[0xc];
    u16 tile;
    u8 pad12[0xa];
    struct Vec position;
    u8 pad24[8];
    struct Animation animation;
    u8 collider[0xa8];
    struct Vec velocity;
    u8 padEC[0x6c];
    u8 frame, speed, trailCount;
    u8 pad15B[3];
    u16 lifetime, initialLifetime;
    u8 turnShift, heading;
    s16 homingInterval;
    u8 pad166[2];
    void *trails[4];
};
extern const s16 gUnk_086149C4[];
extern const u32 gUnk_0824F760[4];
s32 sub_08249350(s32, s32);
void *Player_GetEffectivePosition(s32);
s32 Hresvelgr_GetHomingShotTargetAngle(void *, void *);
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);
s32 Mod(s32, s32);
void *MotionEntry_CreateConfigured(void *, s32, s32, s32, u32, s32);
void EffectEntry_Release(void *);

static inline void SetVelocity(struct Vec *out, u8 angle, u8 speed)
{
    s32 product = gUnk_086149C4[(angle + 64) & 255] * speed;
    s32 x, z;
    if (product >= 0)
        x = product >> 12;
    else
        x = -(-product >> 12);
    out->x = x;
    out->y = 0;
    z = gUnk_086149C4[angle] * speed;
    if (z >= 0)
        z >>= 12;
    else
        z = -(-z >> 12);
    out->z = z;
}
static inline void AddVelocity(struct Vec *out, struct Vec *position, struct Vec *velocity)
{
    u16 x = position->x;
    out->x = velocity->x + x;
    {
        u16 y = position->y;
        out->y = velocity->y + y;
    }
    {
        u16 z = position->z;
        out->z = velocity->z + z;
    }
}
static inline u8 MaskFlags(u8 input, u32 mask)
{
    return input & mask;
}
static inline void Animate(struct Entry *p)
{
    struct Animation *a = &p->animation;
    const volatile u16 *entry = a->data + a->frame;
    p->tile = *entry >> 6;
    {
        u8 input = *(const volatile u8 *)&a->flags;
        u32 mask = 1;
        u32 left = MaskFlags(input, mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->flags |= 4;
        else
            p->flags &= ~4;
    }
    {
        u8 input = *(const volatile u8 *)&a->flags;
        u32 mask = 2;
        u32 left;
        left = MaskFlags(input, mask);
        left = (u8)left;
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->flags |= 8;
        else
            p->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8)
                    a->timer = a->rate;
                else
                    a->frame = a->count - 1;
            } else
                a->frame--;
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                } else
                    a->frame = 0;
            }
        }
        entry = a->data + a->frame;
        a->duration = *entry & 15;
        a->rate = (a->duration * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    }
}
static inline u32 Frame(struct Entry *p)
{
    return p->frame;
}
void EffectPool_UpdateEntry(void *pool, struct Entry *p)
{
    s32 reverse = 0;
    if (sub_08249350(p->frame, p->homingInterval) == 0) {
        s32 diff = Hresvelgr_GetHomingShotTargetAngle(p, Player_GetEffectivePosition(0));
        s32 heading = p->heading;
        u32 angleMask;
        diff -= heading;
        angleMask = 255;
        *(&diff) &= angleMask;
        if (diff > 128) {
            diff = angleMask - diff;
            reverse = 1;
        }
        diff >>= p->turnShift;
        if (reverse == 1)
            diff = heading - diff;
        else
            diff = heading + diff;
        p->heading = diff & angleMask;
    }
    SetVelocity(&p->velocity, p->heading, p->speed);
    AddVelocity(&p->position, &p->position, &p->velocity);
    Animate(p);
    Collider_SetPosition(p->collider, &p->position, 0);
    {
        u32 zero = 0;
        p->collider[0x42] = zero;
    }
    Collider_Refresh(p->collider);
    {
        u8 *trailCount = &p->trailCount;
        if (*trailCount <= 3 && !(Frame(p) & 31)) {
            s32 index;
            u32 types[4];
            s32 angle, sample;
            const u16 *table = gRandomTable;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            sample = table[gRandomIndex];
            index = Mod((sample >> 3) & 15, 4);
            *(struct Types *)types = *(const struct Types *)gUnk_0824F760;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            {
                s32 loaded = table[gRandomIndex];

                sample = loaded;
            }
            angle = ((sample >> 3) & 127) + 64;
            p->trails[*trailCount] =
                MotionEntry_CreateConfigured(&p->position, index, angle, 96, types[index], 200);
            (*trailCount)++;
        }
    }
    if (p->lifetime)
        p->lifetime--;
    if (p->lifetime < (p->initialLifetime >> 3))
        p->flags |= 0x200;
    if (!(p->lifetime & 31)) {
        p->homingInterval = p->homingInterval > 2 ? p->homingInterval - 1 : p->homingInterval;
        p->speed++;
        if (p->turnShift > 2)
            p->turnShift >>= 1;
    }
    if (!p->lifetime)
        EffectEntry_Release(p);
}
