#include "pursuing_actor.h"
#include "player.h"

void TwinSpriteEffect_InitGraphics(struct PursuingActor *effect)
{
    s32 i = 0;
    do {
        struct PursuingActorTrail *sprite = &effect->trail[i];
        void *graphics = sprite->graphics;
        AnimData_Load(graphics, 0x354d);
        Sprite_Init(sprite, graphics, 0);
        AnimData_SelectPaletteBank(graphics, 0x2e2);
        effect->trail[i].sprite.flags = 0x201;
        effect->trail[i].lifetime = 0;
        i++;
    } while (i < 2);
    effect->trailIndex = 0;
    effect->trailPeriod = 2;
}

void TwinSpriteEffect_EmitTrail(struct PursuingActor *p)
{
    if (!Mod(p->timer, p->trailPeriod)) {
        Anim_SetSequence((void *)&p->trail[p->trailIndex].sequence, (void *)p->sequenceResource, 1,
                         0, 0);
        PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->trail[p->trailIndex].sprite,
                               (struct DisplaySequenceState *)&p->trail[p->trailIndex].sequence);
        p->trail[p->trailIndex].sprite.position = p->position;
        p->trail[p->trailIndex].sprite.flags &= ~1;
        p->trail[p->trailIndex].sprite.flags |= 2;
        p->trail[p->trailIndex].sprite.palette = p->phase;
        p->trail[p->trailIndex].lifetime = 4;
        if (++p->trailIndex > 1)
            p->trailIndex = 0;
    }
}

void TimedEffect_UpdateVisibility(struct PursuingActor *effect)
{
    s32 i = 0;
    do {
        if (effect->trail[i].lifetime) {
            if (--effect->trail[i].lifetime == 0)
                effect->trail[i].sprite.flags = (effect->trail[i].sprite.flags | 1) & ~2;
        }
        i++;
    } while (i <= 1);
}

void SwooshProjectile_RemoveTrailSprites(struct PursuingActor *actor)
{
    struct PursuingActorTrail *trail = actor->trail;
    s32 i = 1;
    do {
        Sprite_Remove(trail);
        trail++;
    } while (--i >= 0);
}

void SwooshProjectile_OnPrimaryColliderContact(u32 a, u32 b, struct PursuingActor *actor)
{
    struct PursuingActor *self = actor;
    struct ContactCollider *collider;
    actor->active = 2;
    actor->entered = 1;
    collider = &self->primary;
    collider->flags |= 4;
    collider = &self->secondary;
    collider->flags |= 4;
}

void SlashEffect_OnSecondaryColliderContact(u32 a, u32 b, struct PursuingActor *actor)
{
    struct PursuingActor *self = actor;
    struct ContactCollider *collider;
    actor->active = 2;
    actor->entered = 1;
    collider = &self->primary;
    collider->flags |= 4;
    collider = &self->secondary;
    collider->flags |= 4;
}

void PursuingActor_InitColliders(struct PursuingActor *actor, s32 a, s32 b, s32 c, s32 d,
                                 s32 unused1, s32 unused2, s32 unused3)
{
    struct ActorColliderExtent size, offset;
    void *collider;
    void *position;
    void *secondary;
    size.x = 32;
    size.y = 64;
    size.z = 32;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    collider = &actor->primary;
    Collider_Init(collider, 0, 0x2401, 0, 16, &size, &offset);
    Collider_SetAttackProperties(collider, a, b, 0, c, d);
    Collider_SetCallback(collider, SwooshProjectile_OnPrimaryColliderContact, actor);
    position = &actor->position;
    Collider_SetPosition(collider, position, 0);
    secondary = &actor->secondary;
    Collider_Init(secondary, 0, 0x5000, 0, 16, &size, &offset);
    Collider_SetBodyProperties(secondary, 0, 0, 0);
    Collider_SetCallback(secondary, SlashEffect_OnSecondaryColliderContact, actor);
    Collider_SetPosition(secondary, position, 0);
    Collider_Refresh(secondary);
}

void SlashEffect_UpdateColliders(struct PursuingActor *actor)
{
    struct ContactCollider *primary = &actor->primary;
    struct SceneVec *position = &actor->position;
    Collider_SetPosition(primary, position, 0);
    Collider_SetPosition(&actor->secondary, position, 0);
    Collider_Refresh(primary);
}

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct PursuingActor *SlashEffect_AllocateSlot(struct PursuingActorManager *b)
{
    s32 i;
    struct PursuingActor *e;
    for (i = 0; i < 8; i++) {
        if (!Tst(&b->activeSlots, i)) {
            e = &b->actors[i];
            b->activeSlots |= 1 << i;
            e->slot = i;
            return e;
        }
    }
    return 0;
}

void PursuingActor_TurnTowardPlayer(struct PursuingActor *actor)
{
    struct SceneVec delta;
    struct SceneVec *out;
    struct Player **players;
    struct SceneVec *left, *right;
    s32 angle, difference, magnitude;
    if (actor->turnDelay) {
        actor->turnDelay--;
        return;
    }
    if (!actor->turnTimer)
        return;
    players = gPlayers;
    out = &delta;
    right = &actor->position;
    left = &players[0]->position;
    delta.x = left->x - actor->position.x;
    out->y = left->y - right->y;
    out->z = left->z - right->z;
    angle = Math_Atan2(delta.x, out->z);
    difference = angle - actor->heading;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    magnitude = difference;
    if (difference < 0)
        magnitude = -difference;
    if (magnitude > 8) {
        if (difference > 0)
            actor->heading += actor->turnSpeed;
        else
            actor->heading -= actor->turnSpeed;
    } else {
        actor->turnTimer >>= 1;
        actor->turnSpeed >>= 1;
    }
    if (actor->turnTimer)
        actor->turnTimer--;
    if (Mod(actor->timer, 4) == 0 && actor->turnSpeed)
        actor->turnSpeed--;
}

void PursuingActor_NoOp(void)
{
}

static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline s32 CellIndex(struct SceneVec *p)
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

static inline void AddPosition(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
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

void PursuingActor_AdvanceChase(struct PursuingActorManager *unused, struct PursuingActor *p)
{
    struct SceneVec next;
    struct SceneVec *velocity;
    struct SceneVec *position;
    s32 angle;
    s32 magnitude;
    s32 cell, x, z;
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
        u8 *heading = &p->heading;
        u16 *speed = &p->speed;
        velocity = &p->velocity;
        angle = *heading;
        magnitude = *speed;
    }
    velocity->x = Truncate(gUnk_086149C4[(angle + 64) & 255] * magnitude);
    velocity->y = 0;
    {
        const s16 *sine = gUnk_086149C4;
        u32 index = angle;
        velocity->z = Truncate(sine[index] * magnitude);
    }
    {
        struct SceneVec *out = &next;
        struct SceneVec *current = &p->position;
        struct SceneVec *v = &p->velocity;

        {
            u32 value = (u16)v->x;
            u32 other = (u16)p->position.x;

            next.x = value + other;
        }
        {
            u32 value = (u16)v->y;
            u32 other = (u16)current->y;

            out->y = value + other;
        }
        {
            out->z = (u16)current->z + (u16)v->z;
        }
    }
    x = next.x >> 8;
    z = next.z >> 8;
    position = &p->position;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    if ((((*CellData(cell)) & 15) << 8) > p->position.y) {
        p->active = 2;

        p->entered = 1;
    } else {
        AddPosition(&p->position, &p->velocity, &p->position);
        PursuingActor_TurnTowardPlayer(p);
        SlashEffect_UpdateColliders(p);
        p->phase += 8;
        {
            u16 *frame = &p->timer;
            (*frame)++;
            if (!Mod(*frame, 4)) {
                u16 *speed = &p->speed;
                (*speed)++;
            }
        }
        {
            s32 *lifetime = &p->lifetime;
            if (*lifetime > 0)
                (*lifetime)--;
            else {
                p->active = 3;
                p->entered = 1;
            }
        }
    }
}

static inline void Disable(struct ContactCollider *c)
{
    c->flags |= 4;
}

void PursuingActor_UpdateRecoil(struct PursuingActorManager *pool, struct PursuingActor *p)
{
    struct SceneVec *position;
    struct SceneVec *velocity;
    s32 angle;
    s32 magnitude;
    s32 height;
    if (p->entered) {
        p->timer = 0;
        {
            u32 mask;
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
            const s16 *sine = gUnk_086149C4;
            u32 masked = 255;
            masked &= angle;
            velocity->z = Truncate(sine[masked] * magnitude);
        }
        {
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            {
                u32 offset = index * 2;
                const u16 *source;
                source = (const u16 *)RANDOM_TABLE_ADDR;
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
    AddPosition(&p->position, &p->velocity, &p->position);
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
        pool->activeSlots &= ~(1 << p->slot);
        p->slot = -1;
        p->active = 0;
    } else
        p->timer++;
}

void PursuingActor_UpdateRetirement(struct PursuingActorManager *manager,
                                    struct PursuingActor *actor)
{
    struct SceneVec *position, *velocity;
    if (actor->entered) {
        Disable(&actor->primary);
        Disable(&actor->secondary);
        actor->timer = 0;
        actor->entered = 0;
    }
    position = &actor->position;
    velocity = &actor->velocity;
    actor->position.x = velocity->x + actor->position.x;
    position->y = velocity->y + position->y;
    position->z = velocity->z + position->z;
    TimedEffect_UpdateVisibility(actor);
    actor->phase += 8;
    if (Sprite_BlinkUntilExpired(actor, actor->timer, 10, 30)) {
        Collider_Remove(&actor->secondary);
        SwooshProjectile_RemoveTrailSprites(actor);
        Sprite_Remove(actor);
        manager->activeSlots &= ~(1 << actor->slot);
        actor->slot = -1;
        actor->active = 0;
    } else {
        actor->timer++;
    }
}

void PursuingActor_CreateHomingSlash(const struct SceneVec *position, s32 heading, s32 turnTimer,
                                     s32 turnSpeed, s32 turnDelay, s32 speed, s32 phase, s32 a,
                                     s32 b, s32 c, s32 d)
{
    struct PursuingActorManager *pool;
    struct PursuingActor *slot;
    struct PursuingActor *sprite;
    pool = (void *)gPursuingActorManager;
    if (!pool) {
        pool = SlashEffect_GetOrCreateManager();
        if (!pool)
            return;
    }
    slot = SlashEffect_AllocateSlot(pool);
    if (!slot)
        return;
    slot->active = 1;
    slot->lifetime = 120;
    slot->effectFlags = 0;
    slot->initialHeading = heading;
    slot->heading = heading;
    slot->turnTimer = turnTimer;
    slot->turnSpeed = turnSpeed;
    slot->turnDelay = turnDelay;
    slot->speed = speed;
    slot->entered = 1;
    sprite = slot;
    slot->position = *position;
    slot->sequenceResource = ScriptProc_Start(0x922E, 0x8DC7);
    Sprite_Init(slot, pool->graphics, 0);
    Anim_SetSequence(&slot->sequence, (void *)slot->sequenceResource, 1, 0, 0);
    AnimData_SelectPaletteBank(slot->graphics, 0x2E2);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)slot, &slot->sequence);
    sprite->priority = 1;
    sprite->flags = (sprite->flags | 2) & ~1;
    sprite->phase = phase;
    TwinSpriteEffect_InitGraphics(slot);
    PursuingActor_InitColliders(slot, a, b, c, d, 0, 0, 0);
}

static inline u8 Used(struct PursuingActorManager *pool, u32 bit)
{
    if (pool->activeSlots & bit)
        return 1;
    return 0;
}

s32 PursuingActor_UpdateManager(struct PursuingActorManager *input)
{
    struct PursuingActorManager *pool = input;
    struct PursuingActor *base = pool->actors;
    s32 i;
    if (pool->activeSlots) {
        void **table;
        u8 *state;
        struct PursuingActor *slot;
        i = 0;
        table = gUnk_086107D8;
        state = &pool->actors[0].active;
        slot = base;
        do {
            if (Used(pool, 1U << i))
                sub_08249240(pool, slot, table[*state]);
            {
                u32 stride = sizeof(*slot);
                state = (void *)((u32)state + stride);
                slot++;
            }
            i++;
        } while (i < 8);
    }
    return 0;
}

static inline u8 IsActive(struct PursuingActorManager *manager, s32 index)
{
    if (manager->activeSlots & (1 << index))
        return 1;
    return 0;
}

u32 PursuingActors_ReleaseAll(struct PursuingActorManager *manager)
{
    s32 i, j;
    for (i = 0; i < 8; i++) {
        if (IsActive(manager, i)) {
            struct PursuingActor *actor = &manager->actors[i];
            Collider_Remove(&actor->secondary);
            Sprite_Remove(actor);
            for (j = 0; j < 2; j++) {
                if (manager->actors[i].trail[j].sprite.active)
                    Sprite_Remove(&manager->actors[i].trail[j]);
            }
        }
    }
    return (u32)(gPursuingActorManager = 0);
}

s32 SlashEffect_InitManager(struct PursuingActorManager *s)
{
    s->activeSlots = 0;
    AnimData_Load(s->graphics, 0x0000354D);
    gPursuingActorManager = (void *)s;
    return 0;
}

struct PursuingActorManager *SlashEffect_GetOrCreateManager(void)
{
    struct PursuingActorManager *p;
    if (gPursuingActorManager != 0)
        return (void *)gPursuingActorManager;
    p = Task_Create(0xa, 0xe1c);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))PursuingActor_UpdateManager,
                          (void (*)(void *))PursuingActors_ReleaseAll);
        if (SlashEffect_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void SlashEffect_ResetManagerReference(void)
{
    gPursuingActorManager = 0;
}
