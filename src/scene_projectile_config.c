#include "scene_projectile_config.h"
#include "player_cue_sequence.h"

struct SceneProjectile *Projectile_FindFreeSlot(struct SceneProjectilePool *pool, s32 *out)
{
    struct SceneProjectile *projectile = pool->slots;
    s32 i = 0;
    u32 bit = 1;
    u32 used = pool->used;
    do {
        if ((bit << i & used) != 0) {
            i++;
            projectile++;
            continue;
        }
        *out = i;
        return projectile;
    } while (i <= 9);
    return 0;
}

struct Behaviors {
    void *callback[3];
};

extern const struct Behaviors gUnk_0824FE28;

extern const s16 gUnk_086149C4[];

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

s32 Projectile_SpawnAtPosition(struct SceneProjectileParams *params,
                               struct OrbitEffectPosition *position)
{
    struct Behaviors behaviors = gUnk_0824FE28;
    struct SceneProjectilePool *pool = gSceneProjectilePool;
    struct SceneProjectile *projectile;
    s32 slot;
    u8 *component;
    s32 magnitude;
    s32 angle;
    u32 zero;
    struct OrbitEffectSprite *sprite;
    if (!pool) {
        SceneProjectile_CreatePool();
        return -1;
    }
    projectile = Projectile_FindFreeSlot(pool, &slot);
    if (!projectile)
        return -1;
    Projectile_SetBehaviorCallback(params, behaviors.callback[params->mode]);
    Memory_Copy(&projectile->params, params, sizeof(*params));
    projectile->age = 0;
    component = (u8 *)&projectile->velocity.x;
    angle = params->direction;
    magnitude = params->speed;
    ((struct OrbitEffectPosition *)component)->x =
        Truncate(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff]);
    ((struct OrbitEffectPosition *)component)->y = 0;
    ((struct OrbitEffectPosition *)component)->z =
        Truncate(magnitude * gUnk_086149C4[angle & 0xff]);
    zero = 0;
    component = (u8 *)&projectile->attack;
    Collider_SetAttackProperties(component, params->collisionProperties[2],
                                 params->collisionProperties[3], params->collisionProperties[0],
                                 params->collisionProperties[1], params->collisionProperties[4]);
    ((struct ContactCollider *)component)->bounds = params->bounds;
    ((struct ContactCollider *)component)->offset = params->offset;
    Collider_SetPosition(component, position, 0);
    if (projectile->params.mode == 2) {
        struct ContactCollider *collider = &projectile->body;
        Collider_SetPosition(collider, position, 0);
        Collider_Refresh(collider);
        collider->flags &= 0xFFFB;
    }
    projectile->sprite.position = *position;
    component = (u8 *)&projectile->graphics;
    AnimData_Load(component, params->graphicsId);
    {
        struct OrbitEffectSprite *drawable = &projectile->sprite;
        s32 bank;
        drawable->frame = params->frame;
        bank = params->palette;
        sprite = drawable;
        if (bank >= 0)
            AnimData_SelectPaletteBank(component, bank);
    }
    sprite->flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    if (projectile->params.script) {
        void *proc = ScriptProc_Start(0x922E, (u16)projectile->params.script);
        projectile->scriptProc = proc;
        if (projectile->params.mode == 0) {
            Anim_SetSequence(&projectile->sequence, proc, (u16)projectile->params.sequence, 0,
                             zero);
        } else if (projectile->params.mode == 1) {
            u32 var = (((u16)projectile->params.direction + 0x70) >> 5) & 7;
            Anim_SetSequence(&projectile->sequence, proc, (u16)projectile->params.sequence, var,
                             zero);
        } else {
            u32 var;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            var = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
            var &= 3;
            Anim_SetSequence(&projectile->sequence, proc, (u16)projectile->params.sequence, var,
                             zero);
        }
        {
            struct ContactSwarmSprite *sequenceSprite = (struct ContactSwarmSprite *)sprite;
            PlayerCue_StepSequence(sequenceSprite, &projectile->sequence);
        }
    }
    projectile->launch.speed = params->speed;
    projectile->launch.mode = params->turnSpeed;
    projectile->launch.lifetime = params->lifetime;
    projectile->attack.contactFlags = 0x10;
    projectile->attack.reactionFlags |= 0x100;
    projectile->body.contactFlags = 0x10;
    projectile->params.contactTimer = 0xFFF6;
    projectile->launch.scale = 0x40;
    projectile->sprite.scaleX = 0x40;
    projectile->sprite.scaleY = 0x40;
    projectile->launch.contactCount = params->unknown52;
    pool->used |= 1 << slot;
    sprite->flags &= ~2;
    Sound_PlaySong(0x25A);
    return 0;
}

/* Default configuration for the tentative scene projectile family. */

void Projectile_InitParameters(struct SceneProjectileParams *params)
{
    Memory_Clear(params, sizeof(*params));
    Projectile_SetLaunchParameters(params, 0, 0x3C, 0, 0);
    Projectile_SetCollisionParameters(params, 0, 0, 0xA, 0x14, 0x14);
    Projectile_SetCollisionExtents(params, 0x10, 0x80, 0x10);
    Projectile_SetCollisionOffset_081FD49C(params, 0, 0, 0);
    Projectile_SetSpriteParameters(params, 0x848F, 0, -1);
    Projectile_SetBehaviorCallback(params, SceneProjectile_UpdatePulse);
    params->script = 0;
    params->sequence = 0;
    params->target = 0;
    params->unknown52 = 0;
    params->trackingEnabled = 0;
}

void Projectile_SetLaunchParameters(struct SceneProjectileParams *params, u16 movement,
                                    u16 lifetime, u8 direction, u32 speed)
{
    params->movement = movement;
    params->lifetime = lifetime;
    params->direction = direction;
    params->speed = speed;
}

void Projectile_SetTurnParameters(struct SceneProjectileParams *params, u16 duration, u32 turn,
                                  u8 speed)
{
    params->trackingFrames = duration;
    params->turnParameter = turn;
    params->turnSpeed = speed;
}

void Projectile_SetHomingParameters(struct SceneProjectileParams *params, u16 duration,
                                    const struct ContactPosition *target, u8 speed)
{
    params->trackingFrames = duration;
    params->target = target;
    params->turnSpeed = speed;
}

void Projectile_SetCollisionParameters(struct SceneProjectileParams *params, u32 type, u32 flags,
                                       u32 power, u32 element, u32 duration)
{
    params->collisionProperties[0] = type;
    params->collisionProperties[1] = flags;
    params->collisionProperties[2] = power;
    params->collisionProperties[3] = element;
    params->collisionProperties[4] = duration;
}

void Projectile_SetCollisionExtents(struct SceneProjectileParams *params, u16 x, u16 y, u16 z)
{
    params->bounds.x = x;
    params->bounds.y = y;
    params->bounds.z = z;
}

void Projectile_SetCollisionOffset_081FD49C(struct SceneProjectileParams *params, u16 x, u16 y,
                                            u16 z)
{
    params->offset.x = x;
    params->offset.y = y;
    params->offset.z = z;
}

void Projectile_SetSpriteParameters(struct SceneProjectileParams *params, u32 graphicsId, u32 frame,
                                    u32 palette)
{
    params->graphicsId = graphicsId;
    params->frame = frame;
    params->palette = palette;
}

void Projectile_SetBehaviorCallback(struct SceneProjectileParams *params, void *callback)
{
    params->callback = callback;
}

void Projectile_SetScriptProcedure(struct SceneProjectileParams *params, u32 script, u16 sequence)
{
    params->script = script;
    params->sequence = sequence;
}

void Projectile_SetBehaviorMode(struct SceneProjectileParams *params, u16 mode)
{
    params->mode = mode;
}

s32 Math_Atan2(s32, s32);

void Projectile_AdjustHeading(struct SceneProjectile *slot)
{
    struct SceneProjectileParams *params = &slot->params;
    if (slot->age < params->trackingFrames) {
        const struct ContactPosition *target = params->target;
        if (target) {
            struct OrbitEffectPosition *position = &slot->sprite.position;
            u32 byte = (u8)Math_Atan2(target->x - position->x, target->z - position->z);
            u32 desired;
            u32 current;
            s32 delta;
            u32 step;
            s32 limit;
            u32 flags;
            desired = byte;
            current = (u8)params->direction;
            delta = (s8)desired;
            delta = (s8)(delta - current);
            step = (u8)params->turnSpeed;
            limit = (s8)params->turnSpeed;
            if (delta > limit) {
                delta = current + step;
                params->direction = delta;
            } else if (delta < limit) {
                delta = current - step;
                params->direction = delta;
            } else
                params->direction = desired;
            {
                u32 flag = 1;
                flags = slot->flags;
                slot->flags = flags | flag;
            }
        }
    }
}

void Projectile_SetTrackingTarget(struct SceneProjectileParams *params,
                                  const struct ContactPosition *target)
{
    params->target = target;
    params->trackingEnabled = 1;
}
