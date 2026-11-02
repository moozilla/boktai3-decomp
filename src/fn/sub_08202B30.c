#include "projectile_pool.h"
#include "player_cue_sequence.h"
#include "ram.h"
#include "engine.h"
extern const s16 gUnk_086149C4[];
struct TimedProjectilePool *ProjectilePool_Create(void);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
s32 Sprite_LinkToList(void *, u32);
u8 *SceneBody_GetById(u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
s32 Projectile_SpawnFromConfig(struct ProjectileConfig *params, struct ContactPosition *pos)
{
    struct TimedProjectilePool *pool = gProjectilePool;
    struct ProjectileSlot *e;
    s32 slot;
    u8 *q;
    s32 magnitude;
    s32 angle;
    u32 zero;
    struct DisplaySequenceSprite *o;
    if (!pool) {
        pool = ProjectilePool_Create();
        if (!pool)
            return -1;
    }
    e = ProjectilePool_FindFreeSlot(pool, &slot);
    if (!e)
        return -1;
    Memory_Copy(&e->params, params, sizeof(*params));
    e->timer = 0;
    q = (u8 *)&e->velocity;
    angle = params->angle;
    magnitude = params->speed;
    ((struct ContactPosition *)q)->x = Truncate(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff]);
    ((struct ContactPosition *)q)->y = 0;
    ((struct ContactPosition *)q)->z = Truncate(magnitude * gUnk_086149C4[angle & 0xff]);
    zero = 0;
    q = (u8 *)&e->collider;
    Collider_SetAttackProperties(q, params->power, params->knockback, params->rewardFlags,
                                 params->attackFlags, params->immunity);
    ((struct ContactCollider *)q)->bounds = params->bounds;
    ((struct ContactCollider *)q)->offset = params->offset;
    Collider_SetPosition(q, pos, 0);
    e->sprite.position = *pos;
    q = e->graphics;
    AnimData_Load(q, params->graphicsId);
    {
        struct DisplaySequenceSprite *t = &e->sprite;
        s32 bank;
        t->frame = params->frame;
        bank = params->palette;
        o = t;
        if (bank >= 0)
            AnimData_SelectPaletteBank(q, bank);
    }
    o->flags &= ~1;
    Sprite_LinkToList(o, 0);
    if (e->params.script) {
        e->scriptProc = ScriptProc_Start(0x922E, (u16)e->params.script);
        if (e->params.variants == 0) {
            Anim_SetSequence(&e->anim, e->scriptProc, (u16)e->params.sequence, 0, zero);
        } else if (e->params.variants == 4) {
            u32 var = (((u16)e->params.angle + 0x60) >> 6) & 3;
            Anim_SetSequence(&e->anim, e->scriptProc, (u16)e->params.sequence, var, zero);
        } else {
            u32 var = (((u16)e->params.angle + 0x70) >> 5) & 7;
            Anim_SetSequence(&e->anim, e->scriptProc, (u16)e->params.sequence, var, zero);
        }
        {
            struct ContactSwarmSprite *sprite = (struct ContactSwarmSprite *)o;
            PlayerCue_StepSequence(sprite, &e->anim);
        }
    }
    if (params->homingFrames) {
        u8 *body = SceneBody_GetById(params->bodyId);
        if (body)
            e->target = (void *)(body + 0xc);
    }
    e->params.baseY = e->sprite.position.y;
    if (params->spriteFlag)
        o->flags |= 0x200;
    pool->mask |= 1 << slot;
    return 0;
}
