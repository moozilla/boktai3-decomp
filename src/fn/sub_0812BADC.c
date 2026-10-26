// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "engine.h"
#include "ram.h"

#ifdef NONMATCHING
// fuzzy: 86.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining: sine/RNG load order, position and terrain register allocation, and collider temporaries.
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
    register s32 x asm("r3") = p->x >> 8;
    s32 z = p->z >> 8;
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
        u8 offset = (index * 4) + 12;
        return map->cells + offset;
    }
}

static inline void Disable(struct SlashCollider *c)
{
    c->flags |= 4;
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
// confidence=low: contact reaction reverses horizontal velocity, adds upward motion and retires the slash
void PursuingActor_UpdateRecoil(struct Pool *pool, struct Slash *p)
{
    struct ContactPosition *position;
    struct ContactPosition *velocity;
    s32 angle;
    s32 magnitude;
    s32 height;
    typeof(((u16)p->position.x) + ((u16)velocity->x)) t1;
    if (p->entered) {
        p->timer = 0;
        {
            register u32 mask asm("r1") = 255;
            angle = Math_Atan2(p->velocity.x, p->velocity.z) + 128;
            mask = 255;
            angle &= mask;
        }
        velocity = &p->velocity;
        magnitude = 12;
        {
            const s16 *sine = gUnk_086149C4;
            velocity->x = Truncate(sine[(angle + 64) & 255] * magnitude);
        }
        velocity->y = 0;
        {
            u32 masked = 255;
            masked &= angle;
            velocity->z = Truncate(gUnk_086149C4[masked] * magnitude);
        }
        {
            u16 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            {
                register u32 offset asm("r0") = index * 2;
                const u16 *source;
                source = gUnk_0203B400;
                {
                    u32 sample = *((const u16 *)(((u32)source) + offset));
                    p->velocity.y = (sample & 15) + 8;
                }
            }
        }
        Disable(&p->primary);
        Disable(&p->secondary);
        Sound_PlaySong(0x1E4);
        p->entered = 0;
    }
    TimedEffect_UpdateVisibility(p);
    {
        struct ContactPosition *current = &p->position;
        register struct ContactPosition *velocity asm("r1") = &p->velocity;
        asm("" : "+r"(current), "+r"(velocity));
        t1 = ((u16)p->position.x) + ((u16)velocity->x);
        p->position.x = t1;
        current->y = ((u16)velocity->y) + ((u16)current->y);
        current->z = ((u16)current->z) + ((u16)velocity->z);
    }
    position = &p->position;
    p->velocity.y -= 2;
    height = ((*CellData(CellIndex(position))) & 15) << 8;
    if (p->position.y < height) {
        p->velocity.x = 0;
        p->velocity.y = 0;
        p->velocity.z = 0;
        p->position.y = height;
    }
    if (Sprite_BlinkUntilExpired(p, p->timer, 10, 30)) {
        Collider_Remove(&p->secondary);
        SwooshProjectile_RemoveTrailSprites(p);
        Sprite_Remove(p);
        pool->used &= ~(1 << p->slot);
        p->slot = -1;
        p->state = 0;
    } else
        p->timer++;
}

#else
INCLUDE_ASM("asm/nonmatching", PursuingActor_UpdateRecoil);
#endif
