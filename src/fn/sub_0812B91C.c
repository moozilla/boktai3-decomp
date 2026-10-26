// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "engine.h"
#include "ram.h"

#ifdef NONMATCHING
// fuzzy: 89.0%
// fix.py best version (reprinted C)
// Remaining: heading/velocity temporaries, projected-position lifetime and state-entry tail sharing.
struct SlashCollider {
    u8 pad[6];
    u16 flags;
    u8 unknown8[0x4C];
};
struct Slash {
    u32 flags;
    u8 active, unknown5, phase, priority;
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 trails[0xB8];
    struct SlashCollider primary, secondary;
    struct ContactPosition velocity;
    u8 initialHeading, heading, turnTimer, turnSpeed;
    u16 speed;
    u8 trailIndex, trailPeriod;
    const void *sequenceResource;
    s32 lifetime;
    u16 effectFlags;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered, turnDelay;
};
struct Pool {
    u8 task[0x18];
    u8 graphics[0x20];
    u32 used;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
struct Pool *SlashEffect_GetOrCreateManager(void);
struct Slash *SlashEffect_AllocateSlot(struct Pool *);
void TwinSpriteEffect_InitGraphics(struct Slash *);
void PursuingActor_InitColliders(struct Slash *, s32, s32, s32, s32, s32, s32, s32);
void TimedEffect_UpdateVisibility(struct Slash *);
void TwinSpriteEffect_EmitTrail(struct Slash *);
void PursuingActor_TurnTowardPlayer(struct Slash *);
void SlashEffect_UpdateColliders(struct Slash *);
void SwooshProjectile_RemoveTrailSprites(struct Slash *);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *CellData(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = index * 4 + 12;
        return map->cells + offset;
    }
}
static inline void AddPosition(struct ContactPosition *out, struct ContactPosition *a,
                               struct ContactPosition *b)
{
    {
        u32 v = (u16)a->x;
        out->x = v + (u16)b->x;
    }
    {
        u32 v = (u16)a->y;
        out->y = v + (u16)b->y;
    }
    {
        u32 v = (u16)a->z;
        out->z = v + (u16)b->z;
    }
}
// confidence=low: homing slash travels until a higher terrain cell or its lifetime ends
void PursuingActor_AdvanceChase(struct Pool *unused, struct Slash *p)
{
    struct ContactPosition next;
    struct ContactPosition *velocity;
    struct ContactPosition *position;
    s32 angle;
    s32 magnitude;
    {
        u8 *entered = &p->entered;
        if (*entered) {
            p->timer = 0;
            *entered = 0;
        }
    }
    TwinSpriteEffect_EmitTrail(p);
    TimedEffect_UpdateVisibility(p);
    {
        u32 offset = 0x1A5;
        register u8 *heading asm("r1") = (void *)(((u8 *)p) + offset);
        register u16 *speed asm("r0");
        offset += 3;
        speed = (void *)(((u8 *)p) + offset);
        offset -= 12;
        velocity = (void *)(((u8 *)p) + offset);
        angle = *heading;
        magnitude = *speed;
    }
    velocity->x = Truncate(gUnk_086149C4[(angle + 64) & 255] * magnitude);
    velocity->y = 0;
    {
        const s16 *sine = gUnk_086149C4;
        register u32 index asm("r0") = angle;
        velocity->z = Truncate(sine[index] * magnitude);
    }
    {
        register struct ContactPosition *out asm("r4") = &next;
        register struct ContactPosition *current asm("r3") = &p->position;
        struct ContactPosition *v = &p->velocity;
        asm("" : "+r"(out), "+r"(current), "+r"(v));
        {
            register u32 value asm("r0") = (u16)v->x;
            asm("" : "+r"(value));
            next.x = value + ((u16)p->position.x);
        }
        {
            register u32 value asm("r0") = (u16)v->y;
            asm("" : "+r"(value));
            out->y = value + ((u16)current->y);
        }
        {
            register u32 value asm("r1") = (u16)v->z;
            asm("" : "+r"(value));
            out->z = value + ((u16)current->z);
        }
        position = current;
    }
    if ((((*CellData(CellIndex(&next))) & 15) << 8) > p->position.y) {
        p->state = 2;
        asm("" : : : "memory");
        p->entered = 1;
    } else {
        {
            struct ContactPosition *v = &p->velocity;
            {
                register u32 value asm("r0") = (u16)p->position.x;
                asm("" : "+r"(value));
                p->position.x = value + ((u16)v->x);
            }
            {
                register u32 value asm("r0") = (u16)position->y;
                asm("" : "+r"(value));
                position->y = value + ((u16)v->y);
            }
            {
                register u32 value asm("r0") = (u16)position->z;
                asm("" : "+r"(value));
                position->z = value + ((u16)v->z);
            }
        }
        PursuingActor_TurnTowardPlayer(p);
        SlashEffect_UpdateColliders(p);
        p->phase += 8;
        {
            u16 *frame = &p->timer;
            if (!Mod(*frame, 4)) {
                u16 *speed = &p->speed;
                (*speed)++;
            }
            (*frame)++;
        }
        {
            s32 *lifetime = &p->lifetime;
            if (((u32)(*lifetime)) > 0)
                (*lifetime)--;
            else {
                p->state = 3;
                p->entered = 1;
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", PursuingActor_AdvanceChase);
#endif
