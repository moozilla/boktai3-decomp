#include "vanargand_projectiles.h"

struct VanargandProjectile *
VanargandProjectilePool_FindFreeSlot(struct VanargandProjectilePool *pool, s32 *slotIndex)
{
    s32 index = 0;
    u32 one = 1;
    u32 activeMask = pool->activeMask;
    struct VanargandProjectile *slot = pool->slots;
    struct VanargandProjectile *result;
    do {
        if (!((one << index) & activeMask)) {
            *slotIndex = index;
            result = slot;
            goto end;
        }
        slot++;
        index++;
    } while (index <= 7);
    result = 0;
end:
    return result;
}

void VanargandProjectile_OnContact(struct ContactCollider *actor, void *context,
                                   struct VanargandProjectile *reaction)
{
    reaction->direction = actor->direction;
    Collision_CalculateDamageResponse(actor);
    {
        u32 one = 1;
        if (actor->statRewardFlags & one) {
            Sound_PlaySong(0x592);
            reaction->state = one;
            reaction->entered = one;
            reaction->timer = 0;
            PlayerStats_AddContactHundredths(actor, context, 0x19);
        }
    }
}

void VanargandProjectile_IgnoreContact(void)
{
}

void CpuSet(const void *, void *, u32);

static inline u16 VanargandProjectile_GetResetValue(void)
{
    return 0;
}

static inline void VanargandProjectile_FillPosition(u32 fill, struct ContactPosition *pos)
{
    u32 zero = fill;
    CpuSet((const void *)&zero, pos, 0x05000002);
}

// confidence=low: initializes the six trail sprites and two projectile colliders.

s32 VanargandProjectile_Initialize(struct VanargandProjectilePool *pool,
                                   struct VanargandProjectile *projectile, s32 index)
{
    struct ContactSwarmSprite *sprite;
    struct ContactPosition size, offset;
    struct ContactCollider *collider;
    s32 i;
    u16 reset = VanargandProjectile_GetResetValue();
    projectile->state = 0;
    projectile->entered = 1;
    projectile->timer = reset;
    projectile->direction = 0;
    projectile->targetAngle = reset;
    AnimData_Load(&projectile->graphics, 0x8202);
    AnimData_SelectPaletteBank(&projectile->graphics, 0x1DB);
    sprite = projectile->sprites;
    for (i = 0; i <= 5; i++, sprite++) {
        Sprite_InitUnlinked(sprite, &projectile->graphics, 1);
        sprite->frame = 5 - i;
        sprite->priority = 1;
        VanargandProjectile_FillPosition(0, &sprite->position);
    }
    collider = &projectile->healthCollider;
    size.x = 128;
    size.y = 512;
    size.z = 128;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x4005, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &projectile->position, 0);
    Collider_SetCallback(collider, VanargandProjectile_OnContact, projectile);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    Collider_Refresh(collider);
    collider = &projectile->collider;
    size.x = 16;
    size.y = 100;
    size.z = 16;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &projectile->position, 0);
    Collider_SetCallback(collider, VanargandProjectile_IgnoreContact, projectile);
    Collider_SetAttackProperties(collider, pool->attack, 30, 16, 0, 15);
}

u32 VanargandProjectile_Release(struct VanargandProjectilePool *owner,
                                struct VanargandProjectile *effect, u32 index)
{
    struct ContactSwarmSprite *sprite = effect->sprites;
    s32 count = 5;
    do {
        Sprite_Remove(sprite);
        count--;
        sprite++;
    } while (count >= 0);
    EffectCollision_Disable(&effect->healthCollider);
    owner->activeMask &= ~(1 << index);
}

void VanargandProjectile_UpdatePositionTrail(struct VanargandProjectile *effect)
{
    s32 index, count;
    struct ContactSwarmSprite *sprite;
    if (!(effect->timer & 1))
        effect->sprites[0].position = effect->position;
    index = effect->trailIndex - 1;
    if (index < 0)
        index = 5;
    sprite = &effect->sprites[1];
    count = 4;
    do {
        sprite->position = *(index + effect->history);
        index--;
        if (index < 0)
            index = 5;
        count--;
        sprite++;
    } while (count >= 0);
    if (!(effect->timer & 1)) {
        *(effect->trailIndex + effect->history) = effect->sprites[0].position;
        effect->trailIndex++;
        if (effect->trailIndex > 5)
            effect->trailIndex = 0;
    }
}

extern const s16 gUnk_086149C4[];

static inline void AddOffset(struct ContactPosition *p, struct ContactPosition *v)
{
    p->x += v->x;
    p->y += v->y;
    p->z += v->z;
}

void VanargandProjectile_UpdateRadialTrail(struct VanargandProjectile *effect)
{
    s32 index = effect->trailIndex - 1;
    s32 i;
    struct ContactPosition delta;
    struct ContactPosition *position;
    struct ContactSwarmSprite *sprite;
    if (index < 0)
        index = 5;
    for (i = 0, sprite = effect->sprites; i <= 5; i++, sprite++) {
        position = &sprite->position;
        if (i * 2 > effect->timer) {
            sprite->position = effect->history[index];
            index--;
            if (index < 0)
                index = 5;
        } else {
            MakeOffset(&delta, effect->angles[i], effect->radii[i]);
            AddOffset(position, &delta);
        }
    }
    if (!(effect->timer & 1)) {
        effect->history[effect->trailIndex] = effect->sprites[0].position;
        effect->trailIndex++;
        if (effect->trailIndex > 5)
            effect->trailIndex = 0;
    }
}

void VanargandProjectile_TurnTowardTarget(struct VanargandProjectile *effect)
{
    u16 delta;
    s16 limit;
    effect->targetAngle =
        Math_Atan2(effect->target->x - effect->position.x, effect->target->z - effect->position.z)
        << 8;
    limit = effect->steeringTimer * 12;
    delta = effect->targetAngle - effect->angle;
    if ((s16)delta < -limit)
        delta = -limit;
    else if ((s16)delta > limit)
        delta = limit;
    {
        u32 angle = effect->angle;
        angle += delta;
        effect->angle = angle;
    }
    effect->direction = effect->angle >> 8;
}

void VanargandProjectile_UpdateHomingFlight(void *owner, struct VanargandProjectile *effect,
                                            s32 index)
{
    struct ContactPosition delta;
    if (effect->entered) {
        effect->entered = 0;
        {
            struct ContactCollider *c = &effect->healthCollider;
            s32 mask = -5;
            mask &= c->flags;
            c->flags = mask;
        }
        effect->angle = effect->direction << 8;
        effect->steeringTimer = 64;
    }
    if (effect->steeringTimer)
        effect->steeringTimer--;
    VanargandProjectile_TurnTowardTarget(effect);
    if (effect->steeringTimer > 32) {
        if (effect->speed > 8)
            effect->speed--;
    } else {
        if (effect->speed < 48)
            effect->speed++;
    }
    {
        struct ContactPosition *write = &delta;
        u32 direction = effect->direction;
        u32 speed = effect->speed;
        write->x = VanargandProjectile_Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        write->z = VanargandProjectile_Scale(gUnk_086149C4[direction] * speed);
    }
    {
        struct ContactPosition *position = &effect->position;
        const struct ContactPosition *read = &delta;
        const struct ContactPosition *first = &delta;
        effect->position.x += first->x;
        position->y += read->y;
        position->z += read->z;
        Collider_SetPosition(&effect->collider, position, 0);
        Collider_SetPosition(&effect->healthCollider, position, 0);
    }
    Collider_Refresh(&effect->collider);
    VanargandProjectile_UpdatePositionTrail(effect);
    if (effect->timer > 149) {
        effect->state = 3;
        effect->entered = 1;
        effect->timer = 0;
    } else
        effect->timer++;
}

void VanargandProjectile_UpdateApproachLaunch(void *owner, struct VanargandProjectile *effect,
                                              s32 index)
{
    struct ContactPosition delta;
    if (effect->entered) {
        s32 angle;
        struct ContactCollider *c;
        s32 byte;
        effect->entered = 0;
        c = &effect->healthCollider;
        c->flags |= 4;
        effect->origin = effect->position;
        if (effect->targetSource)
            effect->target = effect->targetSource;
        else
            effect->target = SceneActors_GetAvailablePosition();
        angle = Math_Atan2(effect->target->x - effect->position.x,
                           effect->target->z - effect->position.z);
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 off = gRandomIndex << 1;
            const u8 *table4;
            table4 = (const u8 *)RANDOM_TABLE_ADDR;
            byte = *(const u8 *)(off + (u32)table4);
        }
        if (byte <= 0x7F)
            effect->direction = angle - 0x40;
        else
            effect->direction = angle + 0x40;
        {
            struct ContactCollider *cc = &effect->collider;
            cc->contactFlags = 1;
            cc++;
            (*(&cc))->contactFlags = 1;
        }
    }
    Vec16_Lerp(&effect->position, &effect->origin, effect->target, effect->timer, 5);
    {
        s32 scale = gUnk_086149C4[(effect->timer << 2) & 255] * 3 >> 5;
        struct ContactPosition *write = &delta;
        u32 direction = effect->direction;
        write->x = VanargandProjectile_Scale(scale * gUnk_086149C4[(direction + 64) & 255]);
        write->y = 0;
        write->z = VanargandProjectile_Scale(scale * gUnk_086149C4[direction]);
    }
    {
        struct ContactPosition *position = &effect->position;
        const struct ContactPosition *read = &delta;
        const struct ContactPosition *first = &delta;
        effect->position.x += first->x;
        position->y += read->y;
        position->z += read->z;
        Collider_SetPosition(&effect->collider, position, 0);
        Collider_SetPosition(&effect->healthCollider, position, 0);
    }
    Collider_Refresh(&effect->collider);
    VanargandProjectile_UpdatePositionTrail(effect);
    if (effect->timer > 0x1F) {
        effect->state = 2;
        effect->entered = 1;
        effect->timer = 0;
    } else
        effect->timer++;
}

struct VanargandProjectilePool;

void VanargandProjectile_UpdateRadialExpiry(struct VanargandProjectilePool *owner,
                                            struct VanargandProjectile *effect, u32 index)
{
    if (effect->entered) {
        s32 i;
        effect->entered = 0;
        {
            struct ContactCollider *c = &effect->healthCollider;
            c->flags |= 4;
        }
        {
            struct ContactPosition *tail = &effect->sprites[1].position;
            struct ContactPosition *head = &effect->sprites[0].position;
            effect->direction = Math_Atan2(head->x - tail->x, head->z - tail->z);
        }
        {
            u8 *angles;
            u8 *radii;
            union ParticleRandomTableSource source;
            const u16 *table;
            u32 *randomIndex;
            i = 0;
            angles = effect->angles;
            radii = effect->radii;
            source.halves[0] = (u16)RANDOM_TABLE_ADDR;
            source.halves[1] = RANDOM_TABLE_ADDR >> 16;
            table = ParticleRandom_GetTable(&source);
            randomIndex = &gRandomIndex;
            do {
                angles[i] =
                    (s16)((ParticleRandom_NextSample(table, randomIndex, 1023) & 31) + 240) +
                    effect->direction;
                radii[i] = (ParticleRandom_NextSample(table, randomIndex, 1023) & 15) + 24;
            } while (++i < 6);
        }
    }
    VanargandProjectile_UpdateRadialTrail(effect);
    if (effect->timer > 29)
        VanargandProjectile_Release(owner, effect, index);
    else
        effect->timer++;
}

void VanargandProjectile_UpdateExpiry(void *owner, struct VanargandProjectile *effect, s32 index)
{
    if (effect->entered) {
        effect->entered = 0;
        EffectCollision_Disable(&effect->healthCollider);
    }
    VanargandProjectile_UpdatePositionTrail(effect);
    if (effect->timer > 29)
        VanargandProjectile_Release(owner, effect, index);
    else
        effect->timer++;
}

extern void *gUnk_08612144[];

void sub_08249244(void *, void *, s32, void *);

s32 VanargandProjectilePool_Update(struct VanargandProjectilePool *pool)
{
    struct VanargandProjectile *slot;
    s32 i;
    if (!(gEnginePauseFlags & 7)) {
        slot = pool->slots;
        for (i = 0; i <= 7; i++, slot++) {
            if (pool->activeMask & (1 << i))
                sub_08249244(pool, slot, i, gUnk_08612144[slot->state]);
        }
    }
    return 0;
}

u32 VanargandProjectilePool_Destroy(struct VanargandProjectilePool *pool)
{
    struct VanargandProjectile *projectile = pool->slots;
    s32 i = 0;
    do {
        if (pool->activeMask & (1 << i)) {
            VanargandProjectile_Release(pool, projectile, i);
        }
        Collider_Remove(&projectile->healthCollider);
        i++;
        projectile++;
    } while (i <= 7);
    return 0;
}

s32 VanargandProjectilePool_Init(struct VanargandProjectilePool *pool, s32 attack)
{
    s32 i;
    struct VanargandProjectile *projectile;
    pool->attack = attack;
    projectile = pool->slots;
    for (i = 0; i <= 7; i++, projectile++) {
        VanargandProjectile_Initialize(pool, projectile, i);
    }
    return 0;
}

void *VanargandProjectilePool_Create(s32 attack)
{
    void *pool = Task_Create(9, sizeof(struct VanargandProjectilePool));
    if (pool != 0) {
        Task_SetCallbacks(pool, VanargandProjectilePool_Update, VanargandProjectilePool_Destroy);
        if (VanargandProjectilePool_Init(pool, attack) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}

extern const struct VanargandProjectileProperties gUnk_08612114[];

s32 VanargandProjectile_Create(struct VanargandProjectilePool *pool,
                               struct ContactPosition *position, struct ContactPosition *target,
                               u32 direction, u32 speed, u32 type)
{
    s32 index;
    struct VanargandProjectile *projectile = VanargandProjectilePool_FindFreeSlot(pool, &index);
    struct ContactSwarmSprite *sprite;
    s32 i;
    struct ContactCollider *attack, *body;
    struct ContactPosition *pos;
    if (!projectile)
        return -1;
    {
        u32 zero = 0;
        projectile->state = zero;
        projectile->entered = 1;
        projectile->timer = zero;
        projectile->direction = direction;
        projectile->targetAngle = direction << 8;
        projectile->angle = direction << 8;
        projectile->speed = speed;
        projectile->type = type;
        projectile->steeringTimer = 64;
        projectile->trailIndex = 0;
        projectile->target = target;
        projectile->targetSource = (void *)zero;
    }
    projectile->position = *position;
    sprite = projectile->sprites;
    i = 0;
    attack = &projectile->collider;
    pos = &projectile->position;
    body = &projectile->healthCollider;
initializeSprite:
    *(i + projectile->history) = projectile->position;
    sprite->position = projectile->position;
    sprite->flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    i++;
    sprite++;
    if (i <= 5)
        goto initializeSprite;
    {
        void *animation = &projectile->graphics;
        u32 palette = projectile->type + 0x1db;
        AnimData_SelectPaletteBank(animation, (u16)palette);
    }
    Collider_SetPosition(attack, pos, 0);
    Collider_SetPosition(body, pos, 0);
    body->flags |= 4;
    attack->contactFlags = 16;
    body->contactFlags = 16;
    {
        s32 property = gUnk_08612114[projectile->type].reaction;
        Collider_SetAttackProperties(attack, pool->attack, 30, 16, property, 30);
    }
    Collider_SetBodyProperties(body, 0, gUnk_08612114[projectile->type].reaction,
                               gUnk_08612114[projectile->type].damageScale);
    pool->activeMask |= 1 << index;
    return 0;
}

// confidence=low: the extra source position is retained for the approach-launch state.

#define VanargandProjectile_CreateWithSource sub_081B71D4

s32 VanargandProjectile_CreateWithSource(struct VanargandProjectilePool *pool,
                                         struct ContactPosition *position,
                                         struct ContactPosition *target,
                                         struct ContactPosition *source, u32 direction, u32 speed,
                                         u32 type)
{
    s32 index;
    struct VanargandProjectile *projectile = VanargandProjectilePool_FindFreeSlot(pool, &index);
    struct ContactSwarmSprite *sprite;
    s32 i;
    struct ContactCollider *attack, *body;
    struct ContactPosition *pos;
    if (!projectile)
        return -1;
    {
        u32 zero = 0;
        projectile->state = zero;
        projectile->entered = 1;
        projectile->timer = zero;
        projectile->direction = direction;
        projectile->targetAngle = direction << 8;
        projectile->angle = direction << 8;
        projectile->speed = speed;
        projectile->type = type;
        projectile->steeringTimer = 64;
        projectile->trailIndex = 0;
        projectile->target = target;
    }
    projectile->position = *position;
    projectile->targetSource = source;
    sprite = projectile->sprites;
    i = 0;
    attack = &projectile->collider;
    pos = &projectile->position;
    body = &projectile->healthCollider;
initialize_trail: {
    *(struct ContactPosition *)(i * sizeof(struct ContactPosition) + (u32)projectile->history) =
        projectile->position;
    sprite->position = projectile->position;
    sprite->flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    i++;
    sprite++;
}
    if (i <= 5)
        goto initialize_trail;
    {
        void *animation = &projectile->graphics;
        s16 paletteType = projectile->type;
        AnimData_SelectPaletteBank(animation, paletteType + 0x1db);
    }
    Collider_SetPosition(attack, pos, 0);
    Collider_SetPosition(body, pos, 0);
    body->flags |= 4;
    attack->contactFlags = 16;
    body->contactFlags = 16;
    {
        s32 property = gUnk_08612114[projectile->type].reaction;
        Collider_SetAttackProperties(attack, pool->attack, 30, 16, property, 30);
    }
    Collider_SetBodyProperties(body, 0, gUnk_08612114[projectile->type].reaction,
                               gUnk_08612114[projectile->type].damageScale);
    pool->activeMask |= 1 << index;
    return 0;
}

s32 PooledController_ActivateIdleSlots(struct VanargandProjectilePool *pool)
{
    struct VanargandProjectile *slot = pool->slots;
    s32 i;
    for (i = 0; i <= 7; i++, slot++) {
        if ((pool->activeMask & (1 << i)) && slot->state == 0) {
            slot->state = 3;
            slot->entered = 1;
            slot->timer = 0;
        }
    }
}

struct VanargandAfterimage *
VanargandAfterimagePool_FindFreeSlot(struct VanargandAfterimagePool *pool)
{
    s32 *slot;
    s32 offset;
    s32 end;
    if (pool->count <= 7)
        goto scan;
    goto fail;
found:
    return (struct VanargandAfterimage *)((u8 *)pool->slots + offset);
scan:
    slot = (s32 *)pool->slots;
    offset = 0;
    end = (s32)&pool->slots[7];
    do {
        if (*slot <= 0)
            goto found;
        slot = (s32 *)((struct VanargandAfterimage *)slot + 1);
        offset += sizeof(struct VanargandAfterimage);
    } while ((s32)slot <= end);
fail:
    return 0;
}

void VanargandAfterimagePool_ActivateSlot(struct VanargandAfterimage *afterimage,
                                          struct ContactPosition *position)
{
    afterimage->lifetime = 8;
    afterimage->sprite.flags &= ~0x601;
    afterimage->sprite.position = *position;
}

static inline void CpuSetC(void *dst, u32 control, const void *src)
{
    CpuSet(src, dst, control);
}

void VanargandAfterimagePool_InitializeSlot(void *pool, struct VanargandAfterimage *slot, s32 index)
{
    u32 zero;
    struct ContactSwarmSprite *sprite;
    void *position;
    const void *source;
    slot->lifetime = 0;
    AnimData_Load(&slot->graphics, 0x8202);
    AnimData_SelectPaletteBank(&slot->graphics, 0x6f);
    slot->graphics.paletteIndex = 0x1db;
    sprite = &slot->sprite;
    Sprite_Init(sprite, &slot->graphics, 1);
    sprite->frame = 8;
    sprite->priority = 1;
    position = &slot->sprite.position;
    zero = 0;
    source = (const void *)&zero;
    CpuSetC(position, 0x05000002, source);
}

void VanargandAfterimagePool_ReleaseSlot(struct VanargandAfterimagePool *pool,
                                         struct VanargandAfterimage *afterimage)
{
    afterimage->lifetime = 0;
    afterimage->sprite.flags |= 1;
    pool->count--;
}

void VanargandAfterimagePool_UpdateSlot(struct VanargandAfterimagePool *pool,
                                        struct VanargandAfterimage *afterimage, u32 slotIndex)
{
    if (--afterimage->lifetime == 0)
        VanargandAfterimagePool_ReleaseSlot(pool, afterimage);
    if (afterimage->lifetime == 4) {
        if (slotIndex & 1)
            afterimage->sprite.flags |= 0x200;
        else
            afterimage->sprite.flags |= 0x400;
    }
}

void VanargandAfterimagePool_Update(struct VanargandAfterimagePool *pool)
{
    s32 i;
    struct VanargandAfterimage *afterimage = pool->slots;
    if ((gEnginePauseFlags & 7) == 0) {
        for (i = 0; i <= 7; i++) {
            if (afterimage->lifetime > 0)
                VanargandAfterimagePool_UpdateSlot(pool, &pool->slots[i], i);
            afterimage++;
        }
    }
}

void VanargandAfterimagePool_Destroy(struct VanargandAfterimagePool *pool)
{
    s32 count;
    struct ContactSwarmSprite *sprite = &pool->slots[0].sprite;
    for (count = 7; count >= 0; count--) {
        Sprite_Remove(sprite);
        sprite = (struct ContactSwarmSprite *)((u8 *)sprite + sizeof(struct VanargandAfterimage));
    }
}

s32 VanargandAfterimagePool_Init(struct VanargandAfterimagePool *pool)
{
    s32 i;
    struct VanargandAfterimage *afterimage;
    pool->count = 0;
    for (i = 0, afterimage = pool->slots; i <= 7; i++) {
        VanargandAfterimagePool_InitializeSlot(pool, afterimage, i);
        afterimage++;
    }
    return 0;
}

void *VanargandAfterimagePool_Create(void)
{
    void *pool = Task_Create(9, sizeof(struct VanargandAfterimagePool));
    if (pool != 0) {
        Task_SetCallbacks(pool, VanargandAfterimagePool_Update, VanargandAfterimagePool_Destroy);
        if (VanargandAfterimagePool_Init(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}

s32 VanargandAfterimagePool_Spawn(struct VanargandAfterimagePool *pool, void *position)
{
    void *afterimage = VanargandAfterimagePool_FindFreeSlot(pool);
    if (afterimage != 0) {
        VanargandAfterimagePool_ActivateSlot(afterimage, position);
        pool->count++;
        return 1;
    }
    return 0;
}
