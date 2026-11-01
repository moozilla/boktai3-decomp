#include "global.h"
#include "ram.h"
#include "player_cue_sequence.h"
#include "engine_globals.h"

#define PROJECTILE_RANDOM_TABLE ((const u16 *)0x0203B400)

struct Pos {
    s16 x, y, z, unk_06;
};
struct Pair {
    s32 a, b;
};
struct Obj {
    u32 flags;
    u8 unk_04[3], priority;
    u8 unk_08[8]; /* The first two bytes hold the sprite scales. */
    u16 frame;
    u8 unk_12[10];
    struct Pos pos;
    u8 unk_24[8];
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Params {
    u16 unk_00, lifetime, unk_04;
    s8 angle;
    u8 priority;
    s32 speed;
    u32 unk_0C;
    void *callback;
    u32 attackType, attackFlags;
    s32 attack, element, collisionFlags;
    struct Pair extent, offset;
    u32 unk_38, resource, frame;
    s32 bank;
    u32 script;
    u16 sequence;
    s16 variant;
    u16 unk_50, unk_52, unk_54, unk_56;
};
struct Collider {
    u8 unk_00[6];
    u16 flags;
    u8 unk_08[2];
    u16 unk_0A;
    u8 unk_0C[8];
    struct Pair offset, extent;
    u8 unk_24[0x14];
    u32 flags_38;
    u8 unk_3C[0x18];
};
struct Entry {
    u32 timer;
    u8 unk_04[0xC];
    s16 vx, vy, vz, unk_16;
    struct Anim anim;
    struct Params params;
    u8 animData[0x1C];
    struct Obj sprite;
    struct Collider collider, secondaryCollider;
    s32 speed;
    u8 priority, unk_175;
    u16 lifetime, unk_178, scale;
    void *scriptProc;
    u8 unk_180[0x40];
};
struct Pool {
    u8 unk_00[0x18];
    u32 mask;
};
struct Behaviors {
    void *callback[3];
};
extern const struct Behaviors gUnk_0824FE28;
extern const s16 gUnk_086149C4[];
void SceneProjectile_CreatePool(void);
struct Entry *Projectile_FindFreeSlot(struct Pool *, s32 *);
void Projectile_SetBehaviorCallback(struct Params *, void *);
void Memory_Copy(void *, const void *, s32);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void Collider_SetPosition(void *, void *, u32);
void Sprite_LinkToList(void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);

/* Scale a signed 12-bit fixed-point product toward zero. */
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

s32 Projectile_SpawnAtPosition(struct Params *params, struct Pos *pos)
{
    struct Behaviors behaviors = gUnk_0824FE28;
    struct Pool *pool = gSceneProjectilePool;
    struct Entry *e;
    s32 slot;
    u8 *q;
    s32 magnitude;
    s32 angle;
    u32 zero;
    struct Obj *o;
    if (!pool) {
        SceneProjectile_CreatePool();
        return -1;
    }
    e = Projectile_FindFreeSlot(pool, &slot);
    if (!e)
        return -1;
    Projectile_SetBehaviorCallback(params, behaviors.callback[params->variant]);
    Memory_Copy(&e->params, params, 0x58);
    e->timer = 0;
    q = (u8 *)&e->vx;
    angle = params->angle;
    magnitude = params->speed;
    ((struct Pos *)q)->x = Truncate(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff]);
    ((struct Pos *)q)->y = 0;
    ((struct Pos *)q)->z = Truncate(magnitude * gUnk_086149C4[angle & 0xff]);
    zero = 0;
    q = (u8 *)&e->collider;
    Collider_SetAttackProperties(q, params->attack, params->element, params->attackType,
                                 params->attackFlags, params->collisionFlags);
    ((struct Collider *)q)->extent = params->extent;
    ((struct Collider *)q)->offset = params->offset;
    Collider_SetPosition(q, pos, 0);
    if (e->params.variant == 2) {
        struct Collider *collider = &e->secondaryCollider;
        Collider_SetPosition(collider, pos, 0);
        Collider_Refresh(collider);
        collider->flags &= 0xFFFB;
    }
    e->sprite.pos = *pos;
    q = e->animData;
    AnimData_Load(q, params->resource);
    {
        struct Obj *t = &e->sprite;
        s32 bank;
        t->frame = params->frame;
        bank = params->bank;
        o = t;
        if (bank >= 0)
            AnimData_SelectPaletteBank(q, bank);
    }
    o->flags &= ~1;
    Sprite_LinkToList(o, 0);
    if (e->params.script) {
        void *proc = ScriptProc_Start(0x922E, (u16)e->params.script);
        e->scriptProc = proc;
        if (e->params.variant == 0) {
            Anim_SetSequence(&e->anim, proc, (u16)e->params.sequence, 0, zero);
        } else if (e->params.variant == 1) {
            u32 var = (((u16)e->params.angle + 0x70) >> 5) & 7;
            Anim_SetSequence(&e->anim, proc, (u16)e->params.sequence, var, zero);
        } else {
            u32 var;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            var = PROJECTILE_RANDOM_TABLE[gRandomIndex];
            var &= 3;
            Anim_SetSequence(&e->anim, proc, (u16)e->params.sequence, var, zero);
        }
        {
            struct ContactSwarmSprite *sprite = (struct ContactSwarmSprite *)o;
            PlayerCue_StepSequence(sprite, (struct DisplaySequenceState *)&e->anim);
        }
    }
    e->speed = params->speed;
    e->priority = params->priority;
    e->lifetime = params->lifetime;
    e->collider.unk_0A = 0x10;
    e->collider.flags_38 |= 0x100;
    e->secondaryCollider.unk_0A = 0x10;
    e->params.unk_50 = 0xFFF6;
    e->scale = 0x40;
    e->sprite.unk_08[0] = 0x40;
    e->sprite.unk_08[1] = 0x40;
    e->unk_178 = params->unk_52;
    pool->mask |= 1 << slot;
    o->flags &= ~2;
    Sound_PlaySong(0x25A);
    return 0;
}
