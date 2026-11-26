#include "objects/hazards/animated_hazard.h"
#include "engine/video/display_sequence_inline.h"
#include "scene/terrain/terrain_collision.h"
/* AnimatedHazard_Noop: Empty function with no callers or taken address; placeholder/no-op in the animated-hazard unit. */

struct AnimatedHazardManager *AnimatedHazard_GetManager(void)
{
    return gAnimatedHazardManager;
}

void AnimatedHazard_SetState(void *unused, struct AnimatedHazard *p, u32 v)
{
    p->state = v;
    p->timer = 0;
    p->entered = 1;
}

s32 AnimatedHazard_ConsumeEntered(void *unused, struct AnimatedHazard *p)
{
    s32 r;
    if (p->entered == 0) {
        r = 0;
    } else {
        p->entered = 0;
        r = 1;
    }
    return r;
}

void AnimatedHazard_Initialize(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct HazardExtent size, offset;
    struct DisplaySequenceSprite *sprite = &p->sprite;
    struct ContactCollider *effect;
    Sprite_Init(sprite, &m->animData, 1);
    sprite->priority = 2;
    Anim_SetSequence(&p->anim, m->script, 0, 0, 0);
    effect = &p->collider;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(effect, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(effect, 0, p);
    Collider_SetAttackProperties(effect, 100, 50, 0, 0x60000, 30);
    p->active = 0;
}

void AnimatedHazard_SetState(void *, struct AnimatedHazard *, u32);

void AnimatedHazard_Deactivate(void *a, struct AnimatedHazard *p)
{
    p->sprite.flags |= 1;
    AnimatedHazard_SetState(a, p, 0);
    p->active = 0;
}

void AnimatedHazard_Noop(void)
{
}

void AnimatedHazard_WaitForPlayer(void *m, struct AnimatedHazard *p)
{
    if (gGameState->unk876 > 0)
        p->sprite.flags &= ~1;
    else
        p->sprite.flags |= 1;
    if (++p->timer > p->duration)
        AnimatedHazard_SetState(m, p, 3);
    else {
        struct ContactPosition *player = Player_GetEffectivePosition(0);
        struct ContactPosition *pos = &p->sprite.position;
        int x = player->x - pos->x, z = player->z - pos->z;
        if (x * x + z * z < 14400)
            AnimatedHazard_SetState(m, p, 3);
    }
}

s32 AnimatedHazard_ConsumeEntered(void *, struct AnimatedHazard *);

void AnimatedHazard_EmitWarning(void *m, struct AnimatedHazard *p)
{
    AnimatedHazard_ConsumeEntered(m, p);
    if (p->timer < 60) {
        switch (p->timer) {
            case 0:
            case 24:
            case 48:
                AnimatedParticles_EmitOffset(&p->sprite.position, 1, 3, 0x3000, 64, 8, 256, 256);
                break;
        }
    }
    if (++p->timer > 89)
        AnimatedHazard_SetState(m, p, 3);
}

static inline u32 frame_factor(u16 v)
{
    return DisplaySequence_MaskCommand(v, 15);
}

void AnimatedHazard_AnimateAttack(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct DisplaySequenceSprite *o;
    struct DisplaySequenceState *a;
    const u16 *entry;
    int done;
    if (AnimatedHazard_ConsumeEntered(m, p)) {
        p->sprite.flags &= ~1;
        Anim_SetSequence(&p->anim, m->script, 0, 0, 0);
        Sound_PlaySongChecked(0x34D);
    }
    o = &p->sprite;
    a = &p->anim;
    entry = a->commands + a->index;
    o->frame = *entry >> 6;
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
    if (++a->elapsed >= a->duration) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->index == 0) {
                if (a->flags & 8) {
                    a->elapsed = a->duration;
                } else {
                    a->index = a->frameCount - 1;
                }
                done = 1;
            } else {
                a->index--;
                done = 0;
            }
        } else {
            if (++a->index >= a->frameCount) {
                if ((u8)(a->flags & 8)) {
                    a->elapsed = a->duration;
                    a->index = a->frameCount - 1;
                } else {
                    a->index = 0;
                }
                done = 1;
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        a->frameWeight = frame_factor(*entry);
        a->duration = (a->frameWeight * a->speed) >> 6;
        if (!a->duration)
            a->duration = 1;
    } else
        done = 0;
    if (done)
        AnimatedHazard_SetState(m, p, 4);
    Collider_Refresh(&p->collider);
}

void AnimatedHazard_AnimateHold(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct DisplaySequenceSprite *o;
    struct DisplaySequenceState *a;
    const u16 *entry;
    int done;
    if (AnimatedHazard_ConsumeEntered(m, p)) {
        Anim_SetSequence(&p->anim, m->script, 1, 0, 0);
    }
    o = &p->sprite;
    a = &p->anim;
    entry = a->commands + a->index;
    o->frame = *entry >> 6;
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
    if (++a->elapsed >= a->duration) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->index == 0) {
                if (a->flags & 8) {
                    a->elapsed = a->duration;
                } else {
                    a->index = a->frameCount - 1;
                }
                done = 1;
            } else {
                a->index--;
                done = 0;
            }
        } else {
            if (++a->index >= a->frameCount) {
                if ((u8)(a->flags & 8)) {
                    a->elapsed = a->duration;
                    a->index = a->frameCount - 1;
                } else {
                    a->index = 0;
                }
                done = 1;
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        a->frameWeight = frame_factor(*entry);
        a->duration = (a->frameWeight * a->speed) >> 6;
        if (!a->duration)
            a->duration = 1;
    } else
        done = 0;
    if (p->timer >= p->hold)
        AnimatedHazard_SetState(m, p, 5);
    Collider_Refresh(&p->collider);
}

void AnimatedHazard_Deactivate(void *, struct AnimatedHazard *);

void AnimatedHazard_AnimateRetraction(struct AnimatedHazardManager *m, struct AnimatedHazard *p)
{
    struct DisplaySequenceSprite *o;
    struct DisplaySequenceState *a;
    const u16 *entry;
    int done;
    if (AnimatedHazard_ConsumeEntered(m, p)) {
        Anim_SetSequence(&p->anim, m->script, 2, 0, 0);
    }
    o = &p->sprite;
    a = &p->anim;
    entry = a->commands + a->index;
    o->frame = *entry >> 6;
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
    if (++a->elapsed >= a->duration) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->index == 0) {
                if (a->flags & 8) {
                    a->elapsed = a->duration;
                } else {
                    a->index = a->frameCount - 1;
                }
                done = 1;
            } else {
                a->index--;
                done = 0;
            }
        } else {
            if (++a->index >= a->frameCount) {
                if ((u8)(a->flags & 8)) {
                    a->elapsed = a->duration;
                    a->index = a->frameCount - 1;
                } else {
                    a->index = 0;
                }
                done = 1;
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        a->frameWeight = frame_factor(*entry);
        a->duration = (a->frameWeight * a->speed) >> 6;
        if (!a->duration)
            a->duration = 1;
    } else
        done = 0;
    if (done)
        AnimatedHazard_Deactivate(m, p);
}

extern const u32 gUnk_08604B6C[];

void AnimatedHazard_Update(void *a, struct AnimatedHazard *p)
{
    sub_08249240(a, p, (void *)gUnk_08604B6C[p->state]);
    p->timer += 1;
}

void AnimatedHazard_RemoveSprite(void *unused, struct AnimatedHazard *p)
{
    Sprite_Remove(&p->sprite);
}

void AnimatedHazard_Update(void *, struct AnimatedHazard *);

s32 AnimatedHazardManager_Update(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e = s->hazards;
    s32 i;
    for (i = 7; i >= 0; i--, e++) {
        if (e->active != 0)
            AnimatedHazard_Update(s, e);
    }
    return 0;
}

void AnimatedHazard_RemoveSprite(void *, struct AnimatedHazard *);

u32 AnimatedHazardManager_Destroy(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e = s->hazards;
    s32 i;
    for (i = 7; i >= 0; i--, e++) {
        AnimatedHazard_RemoveSprite(s, e);
    }
    return gAnimatedHazardManager = 0;
}

void AnimatedHazard_Initialize(struct AnimatedHazardManager *, struct AnimatedHazard *);

s32 AnimatedHazardManager_Initialize(struct AnimatedHazardManager *s)
{
    struct AnimatedHazard *e;
    s32 i;
    gAnimatedHazardManager = s;
    s->script = ScriptProc_Start(0x922E, 0xE92A);
    if (AnimData_Load(&s->animData, 0xCD6C) == 0)
        return -1;
    e = s->hazards;
    for (i = 7; i >= 0; i--, e++) {
        AnimatedHazard_Initialize(s, e);
    }
    return 0;
}

s32 AnimatedHazardManager_Initialize(struct AnimatedHazardManager *);

s32 AnimatedHazardManager_Update(struct AnimatedHazardManager *);

u32 AnimatedHazardManager_Destroy(struct AnimatedHazardManager *);

void *AnimatedHazardManager_Create(void)
{
    struct AnimatedHazardManager *p;
    if (gAnimatedHazardManager != 0)
        return gAnimatedHazardManager;
    p = Task_Create(8, sizeof(*p));
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))AnimatedHazardManager_Update,
                          (void (*)(void *))AnimatedHazardManager_Destroy);
        if (AnimatedHazardManager_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

struct AnimatedHazard *AnimatedHazardManager_GetFreeSlot(struct AnimatedHazardManager *p)
{
    struct AnimatedHazard *q = p->hazards;
    s32 i = 0;
    while (i <= 7) {
        if (q->active == 0) {
            return q;
        }
        i++;
        q++;
    }
    return 0;
}

struct AnimatedHazardManager *AnimatedHazard_GetManager(void);

void *AnimatedHazardManager_Create(void);

struct AnimatedHazard *AnimatedHazardManager_GetFreeSlot(struct AnimatedHazardManager *);

static inline struct TerrainCell *AnimatedHazard_GetGroundCell(s32 index)
{
    u8 *override = TerrainModifiers_FindCellOverride(index, 1);
    if (override)
        return &((struct TerrainCellOverride *) override)->cell;
    return &((struct TerrainGrid *)gTerrainMap->cells)->cells[index];
}

int AnimatedHazard_Create(struct ContactPosition *source, int damage, int hold)
{
    struct AnimatedHazardManager *m = AnimatedHazard_GetManager();
    struct AnimatedHazard *p;
    struct ContactPosition *pos;
    struct ContactCollider *effect;
    struct TerrainCell *cell;
    int height, slope;
    if (!m) {
        m = AnimatedHazardManager_Create();
        if (!m)
            return -1;
    }
    p = AnimatedHazardManager_GetFreeSlot(m);
    if (!p)
        return -1;
    p->sprite.position = *source;
    {
        struct ContactPosition *ground = &p->sprite.position;
        int x = ground->x >> 8, z = ground->z >> 8, index;
        pos = ground;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        cell = AnimatedHazard_GetGroundCell(index);
        slope = cell->shapeAndHeight >> 4;
        height = (cell->shapeAndHeight & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)ground->z;
                break;
            case 2:
                height -= (u8)ground->x;
                break;
        }
    }
    p->sprite.position.y = height;
    effect = &p->collider;
    Collider_SetPosition(effect, pos, 0);
    Collider_SetAttackPowerAndForce(effect, (u16)damage, 50);
    AnimatedHazard_SetState(m, p, 2);
    p->timer = 0;
    p->hold = hold;
    p->active = 1;
    return 0;
}

int AnimatedHazard_CreateProximity(struct ContactPosition *source, int damage, int delay, int hold)
{
    struct AnimatedHazardManager *m = AnimatedHazard_GetManager();
    struct AnimatedHazard *p;
    struct ContactPosition *pos;
    struct ContactCollider *effect;
    struct TerrainCell *cell;
    int height, slope;
    if (!m) {
        m = AnimatedHazardManager_Create();
        if (!m)
            return -1;
    }
    p = AnimatedHazardManager_GetFreeSlot(m);
    if (!p)
        return -1;
    p->sprite.position = *source;
    {
        struct ContactPosition *ground = &p->sprite.position;
        int x = ground->x >> 8, z = ground->z >> 8, index;
        pos = ground;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        cell = AnimatedHazard_GetGroundCell(index);
        slope = cell->shapeAndHeight >> 4;
        height = (cell->shapeAndHeight & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)ground->z;
                break;
            case 2:
                height -= (u8)ground->x;
                break;
        }
    }
    p->sprite.position.y = height;
    effect = &p->collider;
    Collider_SetPosition(effect, pos, 0);
    Collider_SetAttackProperties(effect, damage, 50, 0, 8, 30);
    AnimatedHazard_SetState(m, p, 1);
    Anim_SetSequence(&p->anim, m->script, 0, 0, 0);
    {
        struct DisplaySequenceSprite *o = &p->sprite;
        struct DisplaySequenceState *a;
        const u16 *entry;
        int done;
        a = &p->anim;
        entry = a->commands + a->index;
        o->frame = *entry >> 6;
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
        if (++a->elapsed >= a->duration) {
            a->elapsed = 0;
            if (a->flags & 4) {
                if (a->index == 0) {
                    if (a->flags & 8) {
                        a->elapsed = a->duration;
                    } else {
                        a->index = a->frameCount - 1;
                    }
                    done = 1;
                } else {
                    a->index--;
                    done = 0;
                }
            } else {
                if (++a->index >= a->frameCount) {
                    if ((u8)(a->flags & 8)) {
                        a->elapsed = a->duration;
                        a->index = a->frameCount - 1;
                    } else {
                        a->index = 0;
                    }
                    done = 1;
                } else
                    done = 0;
            }
            entry = a->commands + a->index;
            a->frameWeight = frame_factor(*entry);
            a->duration = (a->frameWeight * a->speed) >> 6;
            if (!a->duration)
                a->duration = 1;
        } else
            done = 0;
    }
    AnimData_SelectPaletteBank(&m->animData, 0x33F);
    p->timer = 0;
    p->hold = hold;
    p->duration = delay;
    p->active = 1;
    return 0;
}

s32 AnimatedHazardManager_RetractAll(void)
{
    struct AnimatedHazardManager *s = AnimatedHazard_GetManager();
    struct AnimatedHazard *e;
    s32 i;
    if (s == 0) {
        s = AnimatedHazardManager_Create();
        if (s == 0)
            return -1;
    }
    e = s->hazards;
    for (i = 7; i >= 0; i--) {
        if (e->active != 0 && e->state <= 2)
            AnimatedHazard_SetState(s, e, 3);
        e++;
    }
    return 0;
}
