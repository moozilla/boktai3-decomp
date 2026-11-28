#include "player/player_slot_status.h"
#include "effects/orbits/homing_effect.h"
#include "engine/video/display_sequence_inline.h"
#include "engine/math/math_shift.h"
static inline void EffectManager_SetGlobal(struct EffectManager *manager)
{
    gEffectManager = manager;
}

void EffectManager_ClearGlobal(void)
{
    gEffectManager = NULL;
}

u32 EffectManager_RemoveEffect(struct EffectManager *manager, struct Effect *effect)
{
    s32 i;

    for (i = 0; i < 4; i++)
        Effect_Remove(&effect->particles[i].sprite);
    effect->active = 0;
}

s32 EffectManager_RemoveActiveEffects(void *unused)
{
    struct EffectManager *manager = gEffectManager;
    s32 i;

    if (manager == NULL)
        return -1;
    for (i = 0; i < 6; i++) {
        if (manager->effects[i].active)
            EffectManager_RemoveEffect(manager, &manager->effects[i]);
    }
    return 0;
}

u32 GunDelHell_Create(void *p)
{
    return EffectManager_RemoveActiveEffects(p);
}

s32 EffectManager_UpdateParticles(struct EffectManager *manager, struct Effect *effect,
                                  struct EffectContext *context)
{
    u32 odd = manager->counter & 1, phase = manager->counter & 3;
    s32 i;

    for (i = 0; i < 4; i++) {
        struct EffectParticle *particle = &effect->particles[i];

        gUnk_08604A78[effect->type][particle->state](manager, effect, i, odd, phase, context);
    }
}

/* The isometric screen projection of a world position. */
static inline void projectToScreen(struct EffectVec *screen, const struct EffectVec *world)
{
    s32 x = world->x, z = world->z, sum, row, height;

    screen->x = Math_ShiftTowardZero(3 * (x - z), 5);
    sum = Math_ShiftTowardZero(3 * (x + z), 5);
    row = sum;
    height = Math_ShiftTowardZero(3 * world->y, 5);
    screen->y = row - height;
    screen->z = row + height;
}

s32 EffectManager_UpdateAll(struct EffectManager *manager)
{
    struct EffectContext context;
    struct EffectPackedVec actorWorld;
    struct EffectVec actorScreen;
    struct Effect *effect, *slot;
    s32 i;
    u32 paused = 0x200;
    u32 flags = gEngineFlags | gScenePauseFlags;

    paused &= flags;
    if (paused && !(flags & 0x100))
        return 0;
    actorWorld.x = manager->actor->position.x;
    actorWorld.y = manager->actor->position.y + 128;
    actorWorld.z = manager->actor->position.z;
    projectToScreen(&actorScreen, (struct EffectVec *)&actorWorld);
    slot = manager->effects;
    for (i = 0; i < 6; slot++, i++) {
        effect = slot;
        if (effect->active) {
            if (effect->type == 2) {
                context.world.x = ((struct EffectVec *)&actorWorld)->x;
                context.world.y = ((struct EffectVec *)&actorWorld)->y;
                context.world.z = ((struct EffectVec *)&actorWorld)->z;
            } else if (manager->actor->facing == 0) {
                context.world.x = manager->actor->position.x + 24;
                context.world.y = manager->actor->position.y + 350;
                context.world.z = manager->actor->position.z - 24;
            } else {
                context.world.x = manager->actor->position.x - 32;
                context.world.y = manager->actor->position.y + 320;
                context.world.z = manager->actor->position.z + 32;
            }
            projectToScreen(&context.lower, &context.world);
            context.upper = context.lower;
            context.lower.x -= 5;
            context.lower.y -= 5;
            context.upper.x += 5;
            context.upper.y += 5;
            context.actorLower = actorScreen;
            context.actorUpper = actorScreen;
            context.actorLower.x -= 8;
            context.actorLower.y -= 20;
            context.actorUpper.x += 8;
            context.actorUpper.y += 20;
            EffectManager_UpdateParticles(manager, &manager->effects[i], &context);
            if (effect->soundDelay) {
                effect->soundDelay--;
                if (!effect->soundDelay) {
                    u32 type = effect->type;

                    Sound_PlaySongChecked(gUnk_08604A6C[type]);
                }
            }
            if (effect->timer)
                effect->timer--;
        }
    }
    manager->counter++;
    return 0;
}

s32 EffectManager_DestroyManager(struct EffectManager *manager)
{
    s32 i, j;

    for (i = 0; i < 6; i++) {
        struct Effect *effect = &manager->effects[i];

        if (effect->active) {
            for (j = 0; j < 4; j++)
                Effect_Remove(&effect->particles[j].sprite);
        }
    }
    EffectManager_SetGlobal(NULL);
    return 0;
}

s32 EffectManager_Init(struct EffectManager *manager, void *unused)
{
    struct Player *actor;
    s32 i, j;

    EffectManager_SetGlobal(manager);
    manager->graphics = AnimData_FindEntry(0x1C1A);
    manager->animation = ScriptProc_Start(0x922E, 0xD1B8);
    for (i = 0; i < 6; i++) {
        struct Effect *effect = &manager->effects[i];

        effect->active = 0;
        for (j = 0; j < 4; j++) {
            Effect_Initialize(&effect->particles[j].sprite, manager->graphics, 1);
            Effect_SetPalette(&effect->particles[j].sprite, 1);
        }
    }
    manager->counter = 0;
    actor = gPlayers[0];
    manager->actor = actor;
    if (!actor)
        return -1;
    if (actor->actionMode == 6)
        manager->mode = 1;
    else
        manager->mode = 0;
    return 0;
}

void *EffectManager_Create(void *a, u32 unused)
{
    struct EffectManager *manager = Task_Create(8, sizeof(*manager));

    if (manager != NULL) {
        Task_SetCallbacks(manager, EffectManager_UpdateAll, EffectManager_DestroyManager);
        if (EffectManager_Init(manager, a) < 0) {
            Task_Kill(manager);
            return NULL;
        }
    }
    return manager;
}

s32 Mod(s32, s32);
void sub_08217E40(void *, u32);
extern u32 gUnk_08604A48[];
extern struct EffectAnimId gUnk_08604A18[][3];

static inline u32 spawnRandom(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)table);
}

s32 EffectManager_SpawnParticles(s32 count, u32 type, u32 kind, struct EffectVec *pos,
                                 struct EffectVec *spread, s32 timerBase, s32 timerRange)
{
    u32 lifetime;
    struct EffectManager *manager = gEffectManager;
    s32 reverse, i, n, phase;
    struct Effect *effect;
    if (!manager) {
        manager = EffectManager_Create((void *)0xFC73, 0);
        if (!manager)
            return -1;
    }
    if (!gPlayers[0])
        return 0;
    if (type & 0xF0)
        reverse = 1;
    else
        reverse = 0;
    type &= 15;
    if (count > 4)
        count = 4;
    if (!timerRange)
        timerRange = 1;
    lifetime = gUnk_08604A48[type];
    for (i = 0; i < 6; i++) {
        effect = &manager->effects[i];
        if (effect->active)
            continue;
        effect->type = type;
        effect->kind = kind;
        effect->count = count;
        if (reverse)
            effect->soundDelay = 0;
        else
            effect->soundDelay = 14;
        phase = 0;
        for (n = 0; n < count; n++) {
            struct EffectParticle *p = &effect->particles[n];
            struct EffectSprite *sprite = &p->sprite;
            struct EffectAnim *a;
            const u16 *entry;
            const u16 *table;
            p->state = 1;
            table = (const u16 *)RANDOM_TABLE_ADDR;
            p->timer = timerBase + Mod(spawnRandom(table), timerRange);
            p->lifetime = lifetime;
            p->speed = (spawnRandom(table) & 3) + 4;
            p->angle = spawnRandom(table);
            {
                s32 drift = spawnRandom(table) | -128;
                p->drift = drift;
            }
            p->sprite.flags &= ~1;
            p->sprite.position.x = pos->x;
            if (spread->x)
                p->sprite.position.x += Mod(spawnRandom(table), spread->x);
            p->sprite.position.y = pos->y;
            if (spread->y)
                p->sprite.position.y += Mod(spawnRandom(table), spread->y);
            p->sprite.position.z = pos->z;
            if (spread->z)
                p->sprite.position.z += Mod(spawnRandom(table), spread->z);
            Anim_SetSequence(&p->anim, manager->animation, gUnk_08604A18[manager->mode][type].id, 0,
                             0);
            p->anim.frame = phase;
            if (++phase > 2)
                phase = 0;
            p->velocity.x = 0;
            p->velocity.y = 0;
            p->velocity.z = 0;
            p->origin = p->sprite.position;
            {
                struct EffectGraphics *graphics = manager->graphics;
                a = &p->anim;
                entry = a->data + a->frame;
                {
                    u32 factor = *entry >> 6;
                    p->sprite.tile =
                        (p->sprite.width >> 3) * (p->sprite.height >> 3) * factor + graphics->base;
                }
            }
            if ((u8)(a->flags & 1) != DisplaySequence_MaskCommand((*entry & 0x30) >> 4, 1))
                p->sprite.flags |= 4;
            else
                p->sprite.flags &= ~4;
            if ((u8)(a->flags & 2) != DisplaySequence_MaskCommand((*entry & 0x30) >> 4, 2))
                p->sprite.flags |= 8;
            else
                p->sprite.flags &= ~8;
            if (++a->timer >= a->rate) {
                a->timer = 0;
                if (a->flags & 4) {
                    if (a->frame == 0) {
                        if (a->flags & 8) {
                            a->timer = a->rate;
                        } else {
                            a->frame = a->count - 1;
                        }
                    } else {
                        a->frame--;
                    }
                } else {
                    if (++a->frame >= a->count) {
                        if ((u8)(a->flags & 8)) {
                            a->timer = a->rate;
                            a->frame = a->count - 1;
                        } else {
                            a->frame = 0;
                        }
                    }
                }
                entry = a->data + a->frame;
                a->rate = ((*entry & 15) * a->scale) >> 6;
            }
            sub_08217E40(sprite, 0);
            /* Dead, but it sets table in a second block, so loop.c keeps the load
               in the body as the ROM does (flow drops the store later). */
            table = gRandomTable;
        }
        for (; n <= 3; n++) {
            struct EffectSprite *sprite = &effect->particles[n].sprite;
            sprite->flags |= 1;
            sub_08217E40(sprite, 0);
        }
        effect->timer = 40;
        effect->active = 1;
        break;
    }
    return 0;
}
s32 EffectManager_SpawnParticlesAt(s32 count, u32 type, u32 kind, struct EffectVec *pos,
                                   struct EffectVec *positions, s32 timerBase, s32 timerRange)
{
    u32 lifetime;
    struct EffectManager *manager = gEffectManager;
    s32 reverse, i, n, phase;
    struct Effect *effect;
    if (!manager) {
        manager = EffectManager_Create((void *)0xFC73, 0);
        if (!manager)
            return -1;
    }
    if (type & 0xF0)
        reverse = 1;
    else
        reverse = 0;
    type &= 15;
    if (count > 4)
        count = 4;
    if (!timerRange)
        timerRange = 1;
    lifetime = gUnk_08604A48[type];
    for (i = 0; i < 6; i++) {
        effect = &manager->effects[i];
        if (effect->active)
            continue;
        effect->type = type;
        effect->kind = kind;
        effect->count = count;
        if (reverse)
            effect->soundDelay = 0;
        else
            effect->soundDelay = 14;
        phase = 0;
        for (n = 0; n < count; n++) {
            struct EffectParticle *p = &effect->particles[n];
            struct EffectSprite *sprite = &p->sprite;
            struct EffectAnim *a;
            const u16 *entry;
            const u16 *table;
            const struct EffectVec *src;
            if (positions)
                src = &positions[n];
            else
                src = pos;
            p->state = 1;
            table = gRandomTable;
            p->timer = timerBase + Mod(spawnRandom(table), timerRange);
            p->lifetime = lifetime;
            p->speed = (spawnRandom(table) & 3) + 4;
            p->angle = spawnRandom(table);
            {
                s32 drift = spawnRandom(table) | -128;
                p->drift = drift;
            }
            p->sprite.flags &= ~1;
            p->sprite.position.x = src->x;
            p->sprite.position.y = src->y;
            p->sprite.position.z = src->z;
            Anim_SetSequence(&p->anim, manager->animation, gUnk_08604A18[manager->mode][type].id, 0,
                             0);
            p->anim.frame = phase;
            if (++phase > 2)
                phase = 0;
            p->velocity.x = 0;
            p->velocity.y = 0;
            p->velocity.z = 0;
            p->origin = p->sprite.position;
            {
                struct EffectGraphics *graphics = manager->graphics;
                a = &p->anim;
                entry = a->data + a->frame;
                {
                    u32 factor = *entry >> 6;
                    p->sprite.tile =
                        (p->sprite.width >> 3) * (p->sprite.height >> 3) * factor + graphics->base;
                }
            }
            if ((u8)(a->flags & 1) != DisplaySequence_MaskCommand((*entry & 0x30) >> 4, 1))
                p->sprite.flags |= 4;
            else
                p->sprite.flags &= ~4;
            if ((u8)(a->flags & 2) != DisplaySequence_MaskCommand((*entry & 0x30) >> 4, 2))
                p->sprite.flags |= 8;
            else
                p->sprite.flags &= ~8;
            if (++a->timer >= a->rate) {
                a->timer = 0;
                if (a->flags & 4) {
                    if (a->frame == 0) {
                        if (a->flags & 8) {
                            a->timer = a->rate;
                        } else {
                            a->frame = a->count - 1;
                        }
                    } else {
                        a->frame--;
                    }
                } else {
                    if (++a->frame >= a->count) {
                        if ((u8)(a->flags & 8)) {
                            a->timer = a->rate;
                            a->frame = a->count - 1;
                        } else {
                            a->frame = 0;
                        }
                    }
                }
                entry = a->data + a->frame;
                a->rate = ((*entry & 15) * a->scale) >> 6;
            }
            sub_08217E40(sprite, 0);
            /* Dead, but it sets table in a second block, so loop.c keeps the load
               in the body as the ROM does (flow drops the store later). */
            table = gRandomTable;
        }
        for (; n <= 3; n++) {
            struct EffectSprite *sprite = &effect->particles[n].sprite;
            sprite->flags |= 1;
            sub_08217E40(sprite, 0);
        }
        effect->timer = 40;
        effect->active = 1;
        break;
    }
    return 0;
}
s32 sub_0800B2F4(void)
{
    struct EffectManager *manager = gEffectManager;
    s32 total, i;
    if (manager == NULL)
        return 0;
    total = 0;
    for (i = 0; i < 6; i++) {
        struct Effect *effect = &manager->effects[i];
        if (effect->active)
            total += effect->count;
    }
    return total;
}
void EffectManager_ConvertHomingToDrift(void)
{
    struct EffectManager *manager = gEffectManager;
    s32 i, n;
    if (!manager)
        return;
    for (i = 0; i < 6; i++) {
        struct Effect *effect = &manager->effects[i];
        if (!effect->active || effect->type != 2)
            continue;
        effect->type = 0;
        for (n = 0; n < 4; n++) {
            struct EffectParticle *p = &effect->particles[n];
            if (!p->state)
                continue;
            Anim_SetSequence(&p->anim, manager->animation,
                             gUnk_08604A18[manager->mode][effect->type].id, 0, 0);
            if (p->state <= 2 && p->timer < 180)
                p->timer = 180;
        }
    }
}

void EffectParticle_UpdateNone(void)
{
}

static inline u32 randomValue(s32 reverse)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline s32 scale(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}
static inline s32 scaleHeight(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline void project(struct EffectVec *out, struct EffectVec *in)
{
    s32 x;
    s32 z, h, y;
    x = in->x;
    z = in->z;
    out->x = scale(x - z);
    {
        s32 sum = x + z;
        h = scale(sum);
    }
    y = scaleHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
void EffectParticle_UpdateDriftAroundActor(struct EffectManager *manager, struct Effect *effect,
                                           s32 index, s32 phase, u32 phaseMask,
                                           struct EffectContext *ctx)
{
    struct EffectVec projected;
    struct EffectParticle *p = &effect->particles[index];
    struct EffectVec *origin = &p->origin;
    struct EffectSprite *sprite = &p->sprite, *o;
    u8 *angle = &p->angle;
    struct EffectAnim *a;
    const u16 *entry;
    s32 difference;
    if ((index & 1) == phase) {
        difference =
            Math_Atan2(origin->x - sprite->position.x, origin->z - sprite->position.z) - p->angle;
        if (p->drift < 0) {
            {
                u32 next;
                if ((difference < 0 ? -difference : difference) <= 4) {
                    next = p->angle;
                    next += difference;
                } else
                    next = p->angle + 4;
                *angle = next;
            }
            p->drift++;
            if (!p->drift)
                p->drift = (randomValue(0) & 63) + 64;
        } else {
            {
                u32 next;
                if ((difference < 0 ? -difference : difference) <= 4) {
                    next = p->angle;
                    next -= difference;
                } else
                    next = p->angle - 4;
                *angle = next;
            }
            p->drift--;
            if (!p->drift)
                p->drift = -(randomValue(1) & 63) - 64;
        }
        p->velocity.x = (gUnk_086149C4[(*angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[*angle] * p->speed) >> 12;
    } else if (!effect->timer) {
        project(&projected, &p->sprite.position);
        {
            u32 paused;
            paused = PlayerSlot_ReadStatus(0, 4);
            if (!paused && ctx->actorLower.x <= projected.x && ctx->actorUpper.x >= projected.x &&
                ctx->actorLower.y <= projected.y && ctx->actorUpper.y >= projected.y) {
                p->state = 4;
                Anim_SetSequence(&p->anim, manager->animation,
                                 gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(PlayerSlot_ReadStatus(0, 4))) {
                    Sound_PlaySongChecked(gUnk_08604A60[effect->type]);
                    gUnk_08604A54[effect->type](manager->actor, effect->kind);
                }
                return;
            }
        }
    }
    sprite->position.x = sprite->position.x + p->velocity.x;
    {
        s32 height = ctx->world.y - sprite->position.y;
        if (height > 16)
            sprite->position.y += 16;
        else if (height < -16)
            sprite->position.y -= 16;
        else
            sprite->position.y = ctx->world.y;
    }
    sprite->position.z = sprite->position.z + p->velocity.z;
    o = &p->sprite;
    {
        struct EffectGraphics *graphics = manager->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
            } else {
                a->frame--;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                } else {
                    a->frame = 0;
                }
            }
        }
        entry = a->data + a->frame;
        a->rate = ((*entry & 15) * a->scale) >> 6;
    }
    if (!effect->timer && Player_ReadStatusBits(&manager->actor->statusFlags, 128)) {
        p->state = 2;
        p->speed = 18;
    }
    p->timer--;
    if (!p->timer) {
        p->state = 3;
        p->lifetime = 10;
    }
}

void EffectParticle_UpdateDriftBeforeHoming(struct EffectManager *manager, struct Effect *effect,
                                            s32 index, s32 phase, u32 phaseMask,
                                            struct EffectContext *ctx)
{
    struct EffectVec projected;
    struct EffectParticle *p = &effect->particles[index];
    struct EffectVec *origin = &p->origin;
    struct EffectSprite *sprite = &p->sprite, *o;
    u8 *angle = &p->angle;
    struct EffectAnim *a;
    const u16 *entry;
    s32 difference;
    if ((index & 1) == phase) {
        difference =
            Math_Atan2(origin->x - sprite->position.x, origin->z - sprite->position.z) - p->angle;
        if (p->drift < 0) {
            {
                u32 next;
                if ((difference < 0 ? -difference : difference) <= 4) {
                    next = p->angle;
                    next += difference;
                } else
                    next = p->angle + 4;
                *angle = next;
            }
            p->drift++;
            if (!p->drift)
                p->drift = (randomValue(0) & 63) + 64;
        } else {
            {
                u32 next;
                if ((difference < 0 ? -difference : difference) <= 4) {
                    next = p->angle;
                    next -= difference;
                } else
                    next = p->angle - 4;
                *angle = next;
            }
            p->drift--;
            if (!p->drift)
                p->drift = -(randomValue(1) & 63) - 64;
        }
        p->velocity.x = (gUnk_086149C4[(*angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[*angle] * p->speed) >> 12;
    } else if (!effect->timer) {
        project(&projected, &p->sprite.position);
        {
            u32 paused;
            paused = PlayerSlot_ReadStatus(0, 4);
            if (!paused && ctx->lower.x <= projected.x && ctx->upper.x >= projected.x &&
                ctx->lower.y <= projected.y && ctx->upper.y >= projected.y) {
                p->state = 4;
                Anim_SetSequence(&p->anim, manager->animation,
                                 gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(PlayerSlot_ReadStatus(0, 4))) {
                    Sound_PlaySongChecked(gUnk_08604A60[effect->type]);
                    gUnk_08604A54[effect->type](manager->actor, effect->kind);
                }
                return;
            }
        }
    }
    sprite->position.x = sprite->position.x + p->velocity.x;
    {
        s32 height = ctx->world.y - sprite->position.y;
        if (height > 16)
            sprite->position.y += 16;
        else if (height < -16)
            sprite->position.y -= 16;
        else
            sprite->position.y = ctx->world.y;
    }
    sprite->position.z = sprite->position.z + p->velocity.z;
    o = &p->sprite;
    {
        struct EffectGraphics *graphics = manager->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
            } else {
                a->frame--;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                } else {
                    a->frame = 0;
                }
            }
        }
        entry = a->data + a->frame;
        a->rate = ((*entry & 15) * a->scale) >> 6;
    }
    if (!p->lifetime) {
        p->state = 2;
        p->speed = 15;
        p->lifetime = 10;
        p->timer = (randomValue(0) & 31) + 60;
    } else
        p->lifetime--;
}

static inline u32 randomValueDirect(void)
{
    u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
static inline u32 pausedMode(void)
{
    return PlayerSlot_ReadStatus(0, 4);
}
void EffectParticle_UpdateHomingWide(struct EffectManager *manager, struct Effect *effect,
                                     s32 index, s32 phase, u32 phaseMask, struct EffectContext *ctx)
{
    struct EffectParticle *p = &effect->particles[index];
    struct EffectSprite *sprite = &p->sprite, *o;
    struct EffectAnim *a;
    const u16 *entry;
    s32 dx, dy, dz;
    if ((index & 1) == phase) {
        dx = ctx->world.x - sprite->position.x;
        dy = ctx->world.y - sprite->position.y + 164;
        dz = ctx->world.z - sprite->position.z;
        p->angle = Math_Atan2(dx, dz);
        p->velocity.x = (gUnk_086149C4[(p->angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[p->angle] * p->speed) >> 12;
        {
            if (!(PlayerSlot_ReadStatus(0, 4)) && !effect->timer && (dy < 0 ? -dy : dy) <= 179 &&
                dx * dx + dz * dz < 400) {
                p->state = 4;
                Anim_SetSequence(&p->anim, manager->animation,
                                 gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(PlayerSlot_ReadStatus(0, 4))) {
                    Sound_PlaySongChecked(gUnk_08604A60[effect->type]);
                    gUnk_08604A54[effect->type](manager->actor, effect->kind);
                }
                return;
            }
        }
    }
    sprite->position.x = sprite->position.x + p->velocity.x;
    {
        s32 height = ctx->world.y - sprite->position.y;
        if (height > 16)
            sprite->position.y += 16;
        else if (height < -16)
            sprite->position.y -= 16;
        else
            sprite->position.y = ctx->world.y;
    }
    sprite->position.z = sprite->position.z + p->velocity.z;
    o = &p->sprite;
    {
        struct EffectGraphics *graphics = manager->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
            } else {
                a->frame--;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                } else {
                    a->frame = 0;
                }
            }
        }
        entry = a->data + a->frame;
        a->rate = ((*entry & 15) * a->scale) >> 6;
    }
    if (!Player_ReadStatusBits(&manager->actor->statusFlags, 128) || pausedMode()) {
        p->state = 1;
        p->speed = (randomValueDirect() & 3) + 4;
        p->angle = randomValueDirect();
    }
    p->timer--;
    if (!p->timer) {
        p->state = 3;
        p->lifetime = 10;
    }
}

void EffectParticle_UpdateHoming(struct EffectManager *manager, struct Effect *effect, s32 index,
                                 s32 phase, u32 phaseMask, struct EffectContext *ctx)
{
    struct EffectParticle *p = &effect->particles[index];
    struct EffectSprite *sprite = &p->sprite, *o;
    struct EffectAnim *a;
    const u16 *entry;
    s32 dx, dy, dz;
    if ((index & 1) == phase) {
        dx = ctx->world.x - sprite->position.x;
        dy = ctx->world.y - sprite->position.y + 128;
        dz = ctx->world.z - sprite->position.z;
        p->angle = Math_Atan2(dx, dz);
        p->velocity.x = (gUnk_086149C4[(p->angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[p->angle] * p->speed) >> 12;
        {
            if (!(PlayerSlot_ReadStatus(0, 4)) && !effect->timer && (dy < 0 ? -dy : dy) <= 129 &&
                dx * dx + dz * dz < 400) {
                p->state = 4;
                Anim_SetSequence(&p->anim, manager->animation,
                                 gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(PlayerSlot_ReadStatus(0, 4))) {
                    Sound_PlaySongChecked(gUnk_08604A60[effect->type]);
                    gUnk_08604A54[effect->type](manager->actor, effect->kind);
                }
                return;
            }
        }
    }
    sprite->position.x = sprite->position.x + p->velocity.x;
    {
        s32 height = ctx->world.y - sprite->position.y;
        if (height > 16)
            sprite->position.y += 16;
        else if (height < -16)
            sprite->position.y -= 16;
        else
            sprite->position.y = ctx->world.y;
    }
    sprite->position.z = sprite->position.z + p->velocity.z;
    o = &p->sprite;
    {
        struct EffectGraphics *graphics = manager->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
            } else {
                a->frame--;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                } else {
                    a->frame = 0;
                }
            }
        }
        entry = a->data + a->frame;
        a->rate = ((*entry & 15) * a->scale) >> 6;
    }
    p->timer--;
    if (!p->timer) {
        p->state = 3;
        p->lifetime = 15;
    }
}

void EffectParticle_UpdateFinish(struct EffectManager *manager, struct Effect *effect, s32 index)
{
    struct EffectParticle *p = &effect->particles[index];
    struct EffectSprite *sprite = &p->sprite, *o;
    struct EffectAnim *a;
    const u16 *entry;
    u8 done;
    o = sprite;
    {
        struct EffectGraphics *graphics = manager->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->rate = ((*entry & 15) * a->scale) >> 6;
    } else
        done = 0;
    if (done) {
        p->state = 0;
        sprite->flags |= 1;
        effect->count--;
        if (!effect->count)
            EffectManager_RemoveEffect(manager, effect);
    }
}

void EffectParticle_UpdateDissipating(struct EffectManager *manager, struct Effect *effect,
                                      s32 index, s32 phase)
{
    struct EffectParticle *p = &effect->particles[index];
    struct EffectVec *origin = &p->origin;
    struct EffectSprite *sprite = &p->sprite, *o;
    u8 *angle = &p->angle;
    struct EffectAnim *a;
    const u16 *entry;
    s32 difference;
    if ((index & 1) == phase) {
        difference =
            Math_Atan2(origin->x - sprite->position.x, origin->z - sprite->position.z) - p->angle;
        if (p->drift < 0) {
            {
                u32 next;
                if ((difference < 0 ? -difference : difference) <= 4) {
                    next = p->angle;
                    next += difference;
                } else
                    next = p->angle + 4;
                *angle = next;
            }
            p->drift++;
            if (!p->drift)
                p->drift = (randomValue(0) & 63) + 64;
        } else {
            {
                u32 next;
                if ((difference < 0 ? -difference : difference) <= 4) {
                    next = p->angle;
                    next -= difference;
                } else
                    next = p->angle - 4;
                *angle = next;
            }
            p->drift--;
            if (!p->drift)
                p->drift = -(randomValue(1) & 63) - 64;
        }
        p->velocity.x = (gUnk_086149C4[(*angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[*angle] * p->speed) >> 12;
    }
    sprite->position.x = sprite->position.x + p->velocity.x;
    sprite->position.z = sprite->position.z + p->velocity.z;
    o = &p->sprite;
    {
        struct EffectGraphics *graphics = manager->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
            } else {
                a->frame--;
            }
            entry = a->data + a->frame;
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                } else {
                    a->frame = 0;
                }
            }
            entry = a->data + a->frame;
        }
        a->rate = ((*entry & 15) * a->scale) >> 6;
    }
    if (p->speed > 5)
        p->speed--;
    if (p->timer >= p->lifetime) {
        p->sprite.flags |= 1;
        p->lifetime--;
        p->timer = 0;
    } else
        p->sprite.flags &= ~1;
    p->timer++;
    if (!p->lifetime) {
        p->state = 0;
        sprite->flags |= 1;
        effect->count--;
        if (!effect->count)
            EffectManager_RemoveEffect(manager, effect);
    }
}
