#include "homing_effect.h"
/* EffectManager_ClearGlobal: sets the global EffectManager pointer to 0; matches an existing vetted proposal and the body's single store */
#define EffectManager_ClearGlobal sub_0800A800
// MATCHING_CONCESSION: register-pinning

#define EffectManager_ClearGlobal sub_0800A800
void EffectManager_ClearGlobal(void)
{
    gEffectManager = 0;
}

u32 EffectManager_RemoveEffect(struct EffectManager *manager, struct Effect *effect)
{
    struct EffectSprite *sprite = &effect->particles[0].sprite;
    s32 i;
    for (i = 3; i >= 0; i--) {
        Effect_Remove(sprite);
        sprite = (struct EffectSprite *)((u8 *)sprite + sizeof(struct EffectParticle));
    }
    effect->active = 0;
}

s32 EffectManager_RemoveActiveEffects(void *unused)
{
    struct EffectManager *manager = gEffectManager;
    struct Effect *effect;
    s32 i;
    if (manager == 0)
        return -1;
    effect = manager->effects;
    for (i = 5; i >= 0; i--) {
        if (effect->active != 0)
            EffectManager_RemoveEffect(manager, effect);
        effect++;
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
    u32 one = manager->counter & 1, mask = manager->counter & 3;
    s32 i;
    struct EffectParticle *p;
    i = 0;
    p = effect->particles;
    for (; i < 4; p++, i++)
        gUnk_08604A78[effect->type][p->state](manager, effect, i, one, mask, context);
}

static inline s32 projectScale(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}
static inline s32 projectScaleHeight(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline void projectToScreen(struct EffectVec *out, struct EffectVec *in)
{
    s32 x, z, h, y;
    x = in->x;
    z = in->z;
    out->x = projectScale(x - z);
    h = projectScale(x + z);
    y = projectScaleHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
static inline void projectActorToScreen(struct EffectVec *out, struct EffectVec *in)
{
    s32 x;
    s32 z, h, y;
    x = in->x;
    z = in->z;
    out->x = projectScale(x - z);
    {
        s32 sum = x + z;
        h = projectScale(sum);
    }
    y = projectScaleHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
static inline s32 continuing(s32 i)
{
    register s32 v asm("r0") = i;
    asm("" : : "r"(v));
    return v >= 0;
}
s32 EffectManager_UpdateAll(struct EffectManager *manager)
{
    struct EffectContext context;
    struct EffectContext *ctx;
    struct EffectPackedVec actorWorld;
    struct EffectVec actorScreen;
    struct Effect *p, *view;
    s32 i;
    u32 mask = 512;
    u32 flags;
    flags = gEngineFlags | gScenePauseFlags;
    mask &= flags;
    if (mask) {
        flags &= 256;
        if (!flags)
            goto done;
    }
    actorWorld.x = manager->actor->position.x;
    actorWorld.y = manager->actor->position.y + 128;
    actorWorld.z = manager->actor->position.z;
    projectActorToScreen(&actorScreen, (struct EffectVec *)&actorWorld);
    ctx = &context;
    p = manager->effects;
    view = p;
    i = 5;
    do {
        if (view->active) {
            if (view->type == 2) {
                ctx->world.x = ((struct EffectVec *)&actorWorld)->x;
                ctx->world.y = ((struct EffectVec *)&actorWorld)->y;
                ctx->world.z = ((struct EffectVec *)&actorWorld)->z;
            } else if (manager->actor->facing == 0) {
                ctx->world.x = manager->actor->position.x + 24;
                ctx->world.y = manager->actor->position.y + 350;
                ctx->world.z = manager->actor->position.z - 24;
            } else {
                ctx->world.x = manager->actor->position.x - 32;
                ctx->world.y = manager->actor->position.y + 320;
                ctx->world.z = manager->actor->position.z + 32;
            }
            projectToScreen(&context.lower, &context.world);
            context.upper = context.lower;
            ctx->lower.x -= 5;
            ctx->lower.y -= 5;
            ctx->upper.x += 5;
            ctx->upper.y += 5;
            context.actorLower = actorScreen;
            context.actorUpper = actorScreen;
            ctx->actorLower.x -= 8;
            ctx->actorLower.y -= 20;
            ctx->actorUpper.x += 8;
            ctx->actorUpper.y += 20;
            EffectManager_UpdateParticles(manager, p, &context);
            if (view->soundDelay) {
                view->soundDelay--;
                if (!view->soundDelay) {
                    u32 type = view->type;
                    Sound_PlaySongChecked(gUnk_08604A6C[type]);
                }
            }
            if (view->timer)
                view->timer--;
        }
        {
            register u32 stride asm("r1") = sizeof(*p);
            register s32 step asm("r2");
            view = (struct Effect *)((u8 *)view + stride);
            p = (struct Effect *)((u8 *)p + stride);
            step = -1;
            asm("" : : "r"(step));
            i += step;
        }
    } while (continuing(i));
    {
        struct EffectManager *end = manager;
        end->counter++;
    }
done:
    return 0;
}

s32 EffectManager_DestroyManager(struct EffectManager *manager)
{
    s32 i;
    for (i = 0; i <= 5; i++) {
        struct Effect *effect = &manager->effects[i];
        if (effect->active != 0) {
            struct EffectSprite *sprite = &effect->particles[0].sprite;
            s32 j;
            for (j = 3; j >= 0; j--) {
                Effect_Remove(sprite);
                sprite = (struct EffectSprite *)((u8 *)sprite + sizeof(struct EffectParticle));
            }
        }
    }
    {
        u32 z = 0;
        gEffectManager = (struct EffectManager *)z;
    }
    return 0;
}

s32 EffectManager_Init(struct EffectManager *manager, void *unused)
{
    s32 i, j;
    struct EffectSprite *sprite;
    gEffectManager = manager;
    manager->graphics = AnimData_FindEntry(0x1c1a);
    manager->animation = ScriptProc_Start(0x922e, 0xd1b8);
    for (i = 0; i < 6; i++) {
        struct Effect *effect = &manager->effects[i];
        effect->active = 0;
        {
            register s32 next asm("r7") = i + 1;
        }
        sprite = &effect->particles[0].sprite;
        for (j = 3; j >= 0;
             sprite = (struct EffectSprite *)((u8 *)sprite + sizeof(struct EffectParticle)), j--) {
            Effect_Initialize(sprite, manager->graphics, 1);
            Effect_SetPalette(sprite, 1);
        }
    }
    manager->counter = 0;
    {
        struct Player *actor = ({
            struct Player *a = gPlayers[0];
            a;
        });
        manager->actor = actor;
        if (!actor)
            return -1;
        if (actor->actionMode == 6)
            manager->mode = 1;
        else
            manager->mode = 0;
        return 0;
    }
}

void *EffectManager_Create(void *a, u32 unused)
{
    void *p = Task_Create(8, 0x7DC);
    if (p != 0) {
        Task_SetCallbacks(p, EffectManager_UpdateAll, EffectManager_DestroyManager);
        if (EffectManager_Init(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
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
            {
                u32 left = 1 & a->flags;
                u32 mask = 1;
                if (left != (((*entry & 0x30) >> 4) & mask))
                    p->sprite.flags |= 4;
                else
                    p->sprite.flags &= ~4;
            }
            {
                u32 left = (u8)(2 & a->flags);
                u32 mask = 2;
                if (left != (((*entry & 0x30) >> 4) & mask))
                    p->sprite.flags |= 8;
                else
                    p->sprite.flags &= ~8;
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
            {
                u32 left = 1 & a->flags;
                u32 mask = 1;
                if (left != (((*entry & 0x30) >> 4) & mask))
                    p->sprite.flags |= 4;
                else
                    p->sprite.flags &= ~4;
            }
            {
                u32 left = (u8)(2 & a->flags);
                u32 mask = 2;
                if (left != (((*entry & 0x30) >> 4) & mask))
                    p->sprite.flags |= 8;
                else
                    p->sprite.flags &= ~8;
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
static inline u32 actorMode(struct Player *actor)
{
    u32 mask = 128;
    return actor->statusFlags & mask;
}
void EffectParticle_UpdateDriftAroundActor(struct EffectManager *manager, struct Effect *effect,
                                           s32 index, s32 phase, u32 phaseMask,
                                           struct EffectContext *ctx)
{
    struct EffectVec projected;
    struct EffectParticle *p = &effect->particles[index];
    struct EffectVec *origin = &p->origin;
    struct EffectSprite *sprite = &p->sprite, *o;
    register u8 *angle asm("r6") = &p->angle;
    struct EffectAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    s32 difference;
    if ((index & 1) == phase) {
        difference =
            Math_Atan2(origin->x - sprite->position.x, origin->z - sprite->position.z) - p->angle;
        if (p->drift < 0) {
            if ((difference < 0 ? -difference : difference) <= 4)
                *angle = p->angle + difference;
            else
                *angle = p->angle + 4;
            p->drift++;
            if (!p->drift)
                p->drift = (randomValue(0) & 63) + 64;
        } else {
            if ((difference < 0 ? -difference : difference) <= 4)
                *angle = p->angle - difference;
            else
                *angle = p->angle - 4;
            p->drift--;
            if (!p->drift)
                p->drift = -(randomValue(1) & 63) - 64;
        }
        p->velocity.x = (gUnk_086149C4[(*angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[*angle] * p->speed) >> 12;
    } else if (!effect->timer) {
        project(&projected, &p->sprite.position);
        {
            register u32 mask asm("r8");
            register u32 m asm("r1") = 4;
            u32 paused;
            mask = m;
            paused = gUnk_02000560 & m;
            if (!paused && ctx->actorLower.x <= projected.x && ctx->actorUpper.x >= projected.x &&
                ctx->actorLower.y <= projected.y && ctx->actorUpper.y >= projected.y) {
                p->state = mask;
                Anim_SetSequence(&p->anim, manager->animation,
                                 gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(gUnk_02000560 & mask)) {
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
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        register u32 mask asm("r3") = 2;
        register u32 left asm("r1") = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
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
    if (!effect->timer && actorMode(manager->actor)) {
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
    register u8 *angle asm("r6") = &p->angle;
    struct EffectAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    s32 difference;
    if ((index & 1) == phase) {
        difference =
            Math_Atan2(origin->x - sprite->position.x, origin->z - sprite->position.z) - p->angle;
        if (p->drift < 0) {
            if ((difference < 0 ? -difference : difference) <= 4)
                *angle = p->angle + difference;
            else
                *angle = p->angle + 4;
            p->drift++;
            if (!p->drift)
                p->drift = (randomValue(0) & 63) + 64;
        } else {
            if ((difference < 0 ? -difference : difference) <= 4)
                *angle = p->angle - difference;
            else
                *angle = p->angle - 4;
            p->drift--;
            if (!p->drift)
                p->drift = -(randomValue(1) & 63) - 64;
        }
        p->velocity.x = (gUnk_086149C4[(*angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[*angle] * p->speed) >> 12;
    } else if (!effect->timer) {
        project(&projected, &p->sprite.position);
        {
            u32 mask;
            u32 m = 4;
            u32 paused;
            mask = m;
            paused = gUnk_02000560 & m;
            if (!paused && ctx->lower.x <= projected.x && ctx->upper.x >= projected.x &&
                ctx->lower.y <= projected.y && ctx->upper.y >= projected.y) {
                p->state = mask;
                Anim_SetSequence(&p->anim, manager->animation,
                                 gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(gUnk_02000560 & mask)) {
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
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        register u32 left asm("r1") = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
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
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
void EffectParticle_UpdateHomingWide(struct EffectManager *manager, struct Effect *effect,
                                     s32 index, s32 phase, u32 phaseMask,
                                     struct EffectContext *context)
{
    register struct EffectContext *ctx asm("r10") = context;
    struct EffectParticle *p = &effect->particles[index];
    struct EffectSprite *sprite = &p->sprite, *o;
    struct EffectAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    s32 dx, dy, dz;
    if ((index & 1) == phase) {
        {
            register struct EffectContext *c asm("r2") = ctx;
            dx = c->world.x - sprite->position.x;
        }
        {
            register struct EffectContext *c asm("r3") = ctx;
            dy = c->world.y - sprite->position.y + 164;
            dz = c->world.z - sprite->position.z;
        }
        p->angle = Math_Atan2(dx, dz);
        p->velocity.x = (gUnk_086149C4[(p->angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[p->angle] * p->speed) >> 12;
        {
            u32 mask;
            u32 m = 4;
            mask = m;
            if (!(gUnk_02000560 & m) && !effect->timer && (dy < 0 ? -dy : dy) <= 179 &&
                dx * dx + dz * dz < 400) {
                p->state = mask;
                Anim_SetSequence(&p->anim, manager->animation,
                                 gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(gUnk_02000560 & mask)) {
                    Sound_PlaySongChecked(gUnk_08604A60[effect->type]);
                    gUnk_08604A54[effect->type](manager->actor, effect->kind);
                }
                return;
            }
        }
    }
    sprite->position.x = sprite->position.x + p->velocity.x;
    {
        s32 height;
        register struct EffectContext *c asm("r2") = ctx;
        height = c->world.y - sprite->position.y;
        if (height > 16)
            sprite->position.y += 16;
        else if (height < -16)
            sprite->position.y -= 16;
        else {
            register struct EffectContext *c asm("r3") = ctx;
            sprite->position.y = c->world.y;
        }
    }
    {
        u16 velocity = p->velocity.z;
        u16 position = sprite->position.z;
        sprite->position.z = velocity + position;
    }
    o = &p->sprite;
    {
        register struct EffectManager *m asm("r2") = manager;
        struct EffectGraphics *graphics;
        asm("" : : "r"(m));
        graphics = m->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
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
    if (!actorMode(manager->actor) || pausedMode()) {
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
                                 s32 phase, u32 phaseMask, struct EffectContext *context)
{
    register struct EffectContext *ctx asm("r10") = context;
    struct EffectParticle *p = &effect->particles[index];
    struct EffectSprite *sprite = &p->sprite, *o;
    struct EffectAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    s32 dx, dy, dz;
    if ((index & 1) == phase) {
        {
            register struct EffectContext *c asm("r2") = ctx;
            dx = c->world.x - sprite->position.x;
        }
        {
            register struct EffectContext *c asm("r3") = ctx;
            dy = c->world.y - sprite->position.y + 128;
            dz = c->world.z - sprite->position.z;
        }
        p->angle = Math_Atan2(dx, dz);
        p->velocity.x = (gUnk_086149C4[(p->angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[p->angle] * p->speed) >> 12;
        {
            u32 mask;
            u32 m = 4;
            mask = m;
            if (!(gUnk_02000560 & m) && !effect->timer && (dy < 0 ? -dy : dy) <= 129 &&
                dx * dx + dz * dz < 400) {
                p->state = mask;
                Anim_SetSequence(&p->anim, manager->animation,
                                 gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(gUnk_02000560 & mask)) {
                    Sound_PlaySongChecked(gUnk_08604A60[effect->type]);
                    gUnk_08604A54[effect->type](manager->actor, effect->kind);
                }
                return;
            }
        }
    }
    sprite->position.x = sprite->position.x + p->velocity.x;
    {
        s32 height;
        register struct EffectContext *c asm("r2") = ctx;
        height = c->world.y - sprite->position.y;
        if (height > 16)
            sprite->position.y += 16;
        else if (height < -16)
            sprite->position.y -= 16;
        else {
            register struct EffectContext *c asm("r3") = ctx;
            sprite->position.y = c->world.y;
        }
    }
    {
        u16 velocity = p->velocity.z;
        u16 position = sprite->position.z;
        sprite->position.z = velocity + position;
    }
    o = &p->sprite;
    {
        register struct EffectManager *m asm("r2") = manager;
        struct EffectGraphics *graphics;
        asm("" : : "r"(m));
        graphics = m->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
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
    register u16 n asm("r0");
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
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
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
    register u8 *angle asm("r6") = &p->angle;
    struct EffectAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    s32 difference;
    if ((index & 1) == phase) {
        difference =
            Math_Atan2(origin->x - sprite->position.x, origin->z - sprite->position.z) - p->angle;
        if (p->drift < 0) {
            if ((difference < 0 ? -difference : difference) <= 4)
                *angle = p->angle + difference;
            else
                *angle = p->angle + 4;
            p->drift++;
            if (!p->drift)
                p->drift = (randomValue(0) & 63) + 64;
        } else {
            if ((difference < 0 ? -difference : difference) <= 4)
                *angle = p->angle - difference;
            else
                *angle = p->angle - 4;
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
        register struct EffectManager *m asm("r4") = manager;
        struct EffectGraphics *graphics;
        asm("" : : "r"(m));
        graphics = m->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
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
