#include "global.h"
#include "item_container.h"
// MATCHING_CONCESSION: register-pinning

extern u32 gUnk_02000560[];
extern s32 gUnk_02000478;
extern u16 gUnk_02000534, gUnk_02000500;

void ScaledShadow_Destroy(void *);
s32 sub_0822D490(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void ScaledShadow_Disable(void *);
void FloatingItemEffect_Spawn(s32, s32, s32, s32, struct SceneVec *, struct SceneVec *, s32, struct Trigger *);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
void ParticleSprite_Initialize(struct TriggerSprite *, struct TriggerGraphics *, u32);
void Effect_SetOrigin(struct TriggerSprite *, s32, s32);
s32 Terrain_HasHigherCorner(struct SceneVec *, s32);
s32 Region_FindContaining();
void ScaledShadow_Initialize(void *, struct SceneVec *, s32, s32, s32, s32, s32, s32, s32, s32,
                             s32);

static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
static inline s32 absolute(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}
static inline u8 nearby(struct SceneVec *p, struct SceneVec *a, struct TriggerOpener *actor)
{
    if (absolute(p->x - actor->position.x) <= 95 && absolute(p->y - a->y) <= 127) {
        s32 distance = absolute(p->z - a->z);
        register u32 yes asm("r0") = 1;
        asm("" : : "r"(yes));
        if (distance <= 95)
            return yes;
    }
    return 0;
}
static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 groundHeight(struct SceneVec *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
static inline s32 stateMode(void)
{
    struct Player *state = gUnk_02000580[0];
    if (state) {
        u32 active = state->active;
        s32 yes = 0;
        if (active && state->sceneMode == 4)
            yes = 1;
        return yes;
    }
    return 0;
}
static inline s32 globalMode(void)
{
    u32 active = gUnk_02000534;
    s32 yes = 0;
    if (active && gUnk_02000500 == 4)
        yes = 1;
    return yes;
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
static inline u8 sameRegion(struct Trigger *p)
{
    u32 current = ({
        struct GameState *world = gGameState;
        u32 offset = 0x5A0;
        *(u32 *)((u8 *)world + offset);
    });
    u32 region = p->sound;
    if (current == region)
        return 1;
    return 0;
}

s32 Trigger_RunScript(struct Trigger *input, u32 scriptInput, s32 eventInput)
{
    struct Trigger *p = ({
        struct Trigger *a = input;
        a;
    });
    u16 script;
    s32 event;
    s32 values[10];
    struct TriggerScriptArgs args;
    s32 actor, i;
    s32 savedActor;
    u32 *source;
    s32 *out;
    register struct TriggerScriptArgs *argsPtr asm("r2");
    {
        s32 arg = eventInput;
        event = arg;
    }
    script = scriptInput;
    if (!script)
        goto fail;
    {
        s32 a = p->actor;
        if (a == -1)
            goto fail;
        asm("" : "+r"(a));
        actor = a;
    }
    {
        register s32 index asm("r2") = actor;
        register u32 mask asm("r3") = 12;
        u32 flags;
        flags = ({
                    register u32 *table asm("r1") = gUnk_02000560;
                    table[index];
                }) &
                mask;
        savedActor = index;
        if (flags)
            goto fail;
    }
    goto active;
fail:
    return -1;
active:
    if (p->type == 1) {
        values[0] = p->position->x;
        values[1] = p->position->y + 64;
        values[2] = p->position->z;
    } else {
        values[0] = p->position->x;
        values[1] = p->position->y;
        values[2] = p->position->z;
    }
    values[3] = p->id;
    values[4] = p->kind;
    argsPtr = &args;
    asm("" : : "r"(argsPtr));
    source = p->arguments;
    out = values + 5;
    i = 2;
    do {
        *out++ = *source++;
    } while (--i >= 0);
    values[8] = savedActor;
    values[9] = event;
    {
        u32 mask = 0xffff0000;
        {
            u32 word = *(u32 *)&args & mask;
            word |= 10;
            *(u32 *)&args = word;
        }
    }
    args.values = values;
    argsPtr = &args;
    return Script_Call(script, argsPtr);
}

u32 TriggerManager_InsertTrigger(struct Trigger *p)
{
    struct Trigger *t;
    p->previous = 0;
    p->next = gTriggerManager->head;
    t = p->next;
    if (t) {
        t->previous = p;
    }
    gTriggerManager->head = p;
    return 0;
}
u32 TriggerManager_RemoveTrigger(struct Trigger *p)
{
    struct Trigger *a = p->previous;
    struct Trigger *b = p->next;
    if (a) {
        a->next = b;
    } else {
        gTriggerManager->head = b;
    }
    if (b) {
        b->previous = a;
    }
    return 0;
}
u32 Trigger_Destroy(struct Trigger *s)
{
    TriggerManager_RemoveTrigger(s);
    ScaledShadow_Destroy(s->effect);
    if (s->other != 0)
        Heap_Free(s->other);
    if (s->animation != 0)
        Heap_Free(s->animation);
    if (s->sprite != 0) {
        Effect_Remove(s->sprite);
        Heap_Free(s->sprite);
    }
    if (s->object != 0) {
        Sprite_Remove(s->object);
        Heap_Free(s->object);
    }
    Heap_Free(s);
    return 0;
}
void Trigger_UpdateShake(struct Trigger *p)
{
    if (p->shake) {
        p->shake--;
        if (!p->shake) {
            p->position->x = p->baseX;
            p->position->z = p->baseZ;
        } else {
            const u16 *table = gUnk_0203B400;
            unsigned short random;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            random = table[gRandomIndex];
            {
                struct SceneVec *position = p->position;
                random &= 15;
                position->x = random + p->baseX - 4;
            }
            gRandomIndex = (gRandomIndex + 1) & 1023;
            random = table[gRandomIndex];
            {
                struct SceneVec *position = p->position;
                random &= 7;
                position->z = random + ({
                                  u32 base = (u16)p->baseZ;
                                  base;
                              }) -
                              4;
            }
        }
    }
}

static inline s32 inBox(struct SceneVec *v, struct SceneVec *proj, s16 *o, struct Player *actor, s32 dy)
{
    if (absolute(v->y - actor->position.y) <= 127 && absolute(o[0] - proj->x) <= 14 && dy >= -24) {
        if (dy <= 4)
            return 1;
    }
    return 0;
}

s32 Trigger_UpdateProximity(struct TriggerManager *manager, struct Trigger *p)
{
    s16 box[3];
    struct SceneVec pos;
    s32 armed, i;
    {
        s16 *out = box;
        struct SceneVec *v = p->position;
        s32 x = v->x, z = v->z, a, y;
        out[0] = scale(x - z);
        a = scale(x + z);
        y = scaleHeight(v->y);
        out[1] = a - y;
        out[2] = a + y;
    }
    if (p->cooldown && --p->cooldown == 0) {
        p->shake = 4;
        p->baseX = p->position->x;
        p->baseZ = p->position->z;
    }
    armed = 1;
    for (i = 0; i < 4; i++) {
        struct Player *actor = manager->actors[i];
        struct SceneVec *proj, *v;
        s32 dy, near;
        s16 *o = box;
        if (!actor)
            continue;
        {
            u32 mask = 12;
            if (gUnk_02000560[i] & mask)
                continue;
        }
        v = p->position;
        proj = (struct SceneVec *)(i * 8 + (u32)manager);
        proj = (struct SceneVec *)((u8 *)proj + 60);
        dy = o[1] - proj->y;
        near = inBox(v, proj, o, actor, dy);
        if (!near)
            continue;
        pos = *p->position;
        pos.y += 128;
        if (!(gEngineFlags & 2048) && !sub_0822D490(p->id, p->kind)) {
            if (p->armed && !p->cooldown) {
                if (p->id <= 5) {
                    FloatingItemEffect_Spawn(p->id, p->kind, 1, i, &pos, &pos, 24, p);
                    Sound_PlaySongChecked(272);
                }
                p->cooldown = 36;
                p->shake = 4;
                p->baseX = p->position->x;
                p->baseZ = p->position->z;
            }
        } else {
            p->actor = i;
            if (p->id <= 5)
                FloatingItemEffect_Spawn(p->id, p->kind, 0, i, &pos, &pos, 24, p);
            if (!(gEngineFlags & 2048))
                Sound_PlaySongChecked(272);
            {
                s32 event;
                if (gEngineFlags & 2048)
                    event = 1;
                else
                    event = sub_0822D490(p->id, p->kind) == 1;
                Trigger_RunScript(p, *(u16 *)&p->script, event);
            }
            p->flags |= 1;
            break;
        }
        armed = 0;
    }
    p->armed = armed;
}

s32 Trigger_TryOpenNearby(struct TriggerOpener *actor)
{
    struct TriggerManager *manager = gTriggerManager;
    struct Trigger *p;
    s32 index;
    if (!manager)
        return 0;
    index = actor->index;
    for (p = manager->head; p;) {
        struct Player *state;
        if ((p->flags & 1) || *(u16 *)&p->actor != 511)
            goto next;
        {
            struct SceneVec *point = &actor->position;
            if (!nearby(p->position, point, actor))
                goto next;
        }
        state = gUnk_02000580[index];
        if (state && state->actionMode == 6 && p->condition == 1) {
            if (!manager->soundDelay) {
                Sound_PlaySong(402);
                manager->soundDelay = 15;
            next:
                p = p->next;
            } else {
                p = p->next;
            }
        } else {
            p->actor = index;
            Sound_PlaySongChecked(271);
            p->update = Trigger_UpdateOpeningAnimation;
            p->timer = 0;
            p->started = 0;
            Anim_SetSequence(p->animation, manager->resource, 5, 0, 0);
            {
                s32 event;
                if (gEngineFlags & 2048)
                    event = 1;
                else
                    event = sub_0822D490(p->id, p->kind) == 1;
                Trigger_RunScript(p, *(u16 *)&p->startScript, event);
            }
            return 1;
        }
    }
    return 0;
}

void Trigger_UpdateFalling(struct TriggerManager *manager, struct Trigger *p)
{
    s32 y = p->position->y, height = groundHeight(p->position), difference = y - height;
    if (difference > 0) {
        struct SceneVec *position = p->position;
        s32 step = difference;
        if (step > 32)
            step = 32;
        position->y -= step;
        Trigger_UpdateProximity(manager, p);
        p->timer++;
    } else {
        p->position->y = height + 1;
        p->update = Trigger_UpdateLanding;
        p->timer = 0;
    }
}

void Trigger_UpdateLanding(struct TriggerManager *manager, struct Trigger *p)
{
    s32 height = groundHeight(p->position);
    if (p->timer <= 2)
        p->position->y += 8;
    else if (p->timer <= 5)
        p->position->y -= 8;
    else {
        p->update = Trigger_UpdateGrounded;
        p->timer = 0;
        p->position->y = height + 1;
        ScaledShadow_Disable(p->effect);
        return;
    }
    Trigger_UpdateShake(p);
    Trigger_UpdateProximity(manager, p);
    p->timer++;
}

void Trigger_UpdateBobbingAnimation(struct TriggerManager *manager, struct Trigger *p)
{
    struct TriggerObject *o;
    struct TriggerAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    o = p->object;
    a = p->animation;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
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
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    }
    if (p->flip)
        p->object->flags |= 4;
    if (p->timer <= 3)
        p->position->y += 8;
    else if (p->timer <= 7)
        p->position->y -= 8;
    else {
        p->update = Trigger_UpdateIdleAnimation;
        p->timer = 0;
        return;
    }
    p->timer++;
}

void Trigger_UpdateGrounded(struct TriggerManager *manager, struct Trigger *p)
{
    if (!p->persistent && p->timer > 899) {
        p->update = Trigger_UpdateDropTimer;
        p->timer = 0;
    } else {
        if ((p->timer & 15) == 15) {
            if (TerrainModifiers_FindCellOverride(cellIndex(p->position), 1))
                p->timer = 900;
        }
        Trigger_UpdateShake(p);
        Trigger_UpdateProximity(manager, p);
        p->timer++;
    }
}

void Trigger_UpdateDropTimer(struct TriggerManager *a, struct Trigger *b)
{
    if (Sprite_BlinkUntilExpired(b->sprite, b->timer, 0x2d, 0x5a))
        b->flags |= 1;
    else
        Trigger_UpdateProximity(a, b);
    b->timer++;
}

void Trigger_UpdateIdleAnimation(struct TriggerManager *manager, struct Trigger *p)
{
    struct TriggerObject *o;
    struct TriggerAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    o = p->object;
    a = p->animation;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
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
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    }
    if (p->flip)
        p->object->flags |= 4;
    p->timer++;
}

void Trigger_UpdateOpeningAnimation(struct TriggerManager *manager, struct Trigger *p)
{
    struct TriggerObject *o;
    struct TriggerAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    struct SceneVec raised;
    o = p->object;
    a = p->animation;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
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
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    if (p->flip)
        p->object->flags |= 4;
    if (done) {
        raised = *p->position;
        raised.y += 128;
        if (!(gEngineFlags & 2048) && sub_0822D490(p->id, p->kind) == 0) {
            if (p->id <= 5)
                FloatingItemEffect_Spawn(p->id, p->kind, 1, p->actor, &raised, &raised, 24, p);
            p->update = Trigger_UpdateLockedPause;
        } else {
            if (p->id <= 5)
                FloatingItemEffect_Spawn(p->id, p->kind, 0, p->actor, &raised, &raised, 24, p);
            p->update = Trigger_UpdateActivation;
        }
        p->timer = 0;
        Anim_SetSequence(p->animation, gTriggerManager->resource, 6, 0, 0);
    } else
        p->timer++;
}

void Trigger_UpdateActivation(struct TriggerManager *manager, struct Trigger *p)
{
    struct TriggerObject *o;
    struct TriggerAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    o = p->object;
    a = p->animation;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
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
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    }
    if (p->flip)
        p->object->flags |= 4;
    if (p->timer == 8) {
        u16 script;
        if (!(gEngineFlags & 2048))
            Sound_PlaySongChecked(272);
        script = *(u16 *)&p->script;
        Trigger_RunScript(p, script, sub_0822D490(p->id, p->kind) == 1);
    } else if (p->timer > 8) {
        if (Sprite_BlinkUntilExpired(p->object, p->timer, 30, 90))
            p->flags |= 1;
    }
    p->timer++;
}

void Trigger_UpdateLockedPause(struct TriggerManager *manager, struct Trigger *p)
{
    struct TriggerObject *o;
    struct TriggerAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    o = p->object;
    a = p->animation;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
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
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    }
    if (p->flip)
        p->object->flags |= 4;
    if (p->timer > 31) {
        p->update = Trigger_UpdateLockedRecovery;
        p->timer = 0;
        Anim_SetSequence(p->animation, gTriggerManager->resource, 5, 0, 4);
    }
    p->timer++;
}

void Trigger_UpdateLockedRecovery(struct TriggerManager *manager, struct Trigger *p)
{
    struct TriggerObject *o;
    struct TriggerAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    o = p->object;
    a = p->animation;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
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
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    if (p->flip)
        p->object->flags |= 4;
    if (done) {
        p->update = Trigger_UpdateIdleAnimation;
        p->timer = 0;
        p->actor = -1;
        Anim_SetSequence(p->animation, manager->resource, 0, 0, 0);
    } else
        p->timer++;
}

s32 TriggerManager_Update(struct TriggerManager *input)
{
    struct TriggerManager *p = input;
    s32 i;
    struct Trigger *trigger, *next;
    u8 *delay;
    {
        struct Player **table;
        i = 0;
        table = gUnk_02000580;
        for (; i < 4; i++) {
            struct Player *actor;
            asm("" : "+r"(i));
            actor = ({
                struct Player *a = table[i];
                a;
            });
            p->actors[i] = actor;
            if (actor) {
                u32 offset = i * 8;
                struct SceneVec *out;
                struct SceneVec *in;
                s32 x, z, horizontal, height;
                out = (struct SceneVec *)(offset + (u32)({
                                              struct TriggerManager *manager = p;
                                              manager;
                                          }));
                out = (struct SceneVec *)((u8 *)out + 60);
                in = &actor->position;
                x = ({
                    register s32 value asm("r4") = actor->position.x;
                    asm("" : : "r"(value));
                    value;
                });
                z = in->z;
                out->x = scale(x - z);
                horizontal = scale(({
                    register s32 sum asm("r1") = x + z;
                    asm("" : : "r"(sum));
                    sum;
                }));
                height = scaleHeight(in->y);
                out->y = horizontal - height;
                out->z = horizontal + height;
            }
        }
    }
    trigger = ({
        struct TriggerManager *m = p;
        m->head;
    });
    delay = ({
        u8 *value = (u8 *)36;
        value = (u8 *)((u32)value + (u32)p);
        value;
    });
    for (; trigger; trigger = next) {
        next = trigger->next;
        if (trigger->flags & 1)
            Trigger_Destroy(trigger);
        else {
            if (!trigger->sound || sameRegion(trigger)) {
                if (trigger->sprite)
                    trigger->sprite->flags &= ~1;
                {
                    struct TriggerObject *object;
                    struct TriggerObject *candidate = ({
                        struct TriggerObject *value = trigger->object;
                        value;
                    });
                    object = candidate;
                    if (candidate) {
                        if (!trigger->started)
                            goto showObject;
                        else if (gUnk_02000478 > 0) {
                            candidate->flags &= ~1;
                            trigger->flash = 0;
                            trigger->tick = 0;
                            trigger->visible = 1;
                        } else {
                            s32 available;
                            if (gEngineFlags & 2048)
                                available = stateMode();
                            else if (gUnk_02000580[0] && ({
                                         u32 *flags = &gUnk_02000580[0]->actionFlags;
                                         u32 mask = 16;
                                         *flags &mask;
                                     }))
                                goto available;
                            else
                                available = globalMode();
                            asm("" : : "r"(available));
                            if (available) {
                            available:
                                if (trigger->visible)
                                    trigger->flash = 31;
                                {
                                    u32 ticks = trigger->flash;
                                    register u32 saved asm("r2") = ticks;
                                    if (saved) {
                                        ticks--;
                                        trigger->flash = ticks;
                                        ticks &= 15;
                                        if (ticks > 7)
                                            object->flags &= ~1;
                                        else
                                            object->flags |= 1;
                                    } else {
                                        if (trigger->tick <= 47)
                                            object->flags &= ~1;
                                        else if (trigger->tick <= 71)
                                            object->flags |= 1;
                                        else
                                            trigger->tick = saved;
                                        trigger->tick++;
                                    }
                                }
                                trigger->visible = 0;
                            } else {
                                if (!trigger->visible)
                                    trigger->flash = 47;
                                {
                                    u32 ticks = trigger->flash;
                                    if (ticks) {
                                        ticks--;
                                        trigger->flash = ticks;
                                        if ((ticks & 15) > 7)
                                            object->flags |= 1;
                                        else
                                            object->flags &= ~1;
                                        trigger->tick = 0;
                                    } else
                                        object->flags |= 1;
                                }
                                trigger->visible = 1;
                            }
                        }
                    }
                    goto updatedVisibility;
                showObject:
                    candidate->flags &= ~1;
                updatedVisibility:;
                }
            } else {
                if (trigger->sprite)
                    trigger->sprite->flags |= 1;
                if (trigger->object)
                    trigger->object->flags |= 1;
            }
            sub_08249240(p, trigger, trigger->update);
        }
    }
    {
        u8 *timer = ({
            u8 *value = delay;
            value;
        });
        u32 ticks = *timer;
        if (ticks)
            *timer = ticks - 1;
    }
    return 0;
}

u32 TriggerManager_DestroyManager(struct TriggerManager *s)
{
    struct Trigger *n = s->head;
    while (n != 0) {
        struct Trigger *next = n->next;
        Trigger_Destroy(n);
        n = next;
    }
    gTriggerManager = 0;
    return 0;
}
s32 TriggerManager_Initialize(struct TriggerManager *p, void *unused)
{
    s32 i;
    gTriggerManager = p;
    p->graphics = AnimData_FindEntry(7194);
    p->kind = 14454;
    p->resource = ScriptProc_Start(37422, 39666);
    p->head = 0;
    for (i = 0; i < 4; i++) {
        struct Player *actor;
        actor = ({
            struct Player *a = gUnk_02000580[i];
            a;
        });
        p->actors[i] = actor;
        if (actor) {
            u32 offset = i * 8;
            struct SceneVec *out;
            struct SceneVec *in;
            s32 x, z, horizontal, height;
            out = (struct SceneVec *)(offset + (u32)p);
            out = (struct SceneVec *)((u8 *)out + 60);
            in = &actor->position;
            x = ({
                s32 value = actor->position.x;
                value;
            });
            *(&z) = in->z;
            out->x = scale(x - z);
            horizontal = scale(({
                s32 sum = x + z;
                sum;
            }));
            height = scaleHeight(in->y);
            out->y = horizontal - height;
            out->z = horizontal + height;
        }
    }
    return 0;
}

void *TriggerManager_InitManager(void *a, void *b)
{
    void *p = Task_Create(9, 0x5c);
    if (p != 0) {
        Task_SetCallbacks(p, TriggerManager_Update, TriggerManager_DestroyManager);
        if (TriggerManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

struct TriggerManager *TriggerManager_GetOrCreate(void)
{
    if (gTriggerManager == 0) {
        return TriggerManager_InitManager(0, 0);
    }
    return gTriggerManager;
}

s32 Trigger_CreateDrop(struct SceneVec *position, s32 id, s32 kind, u32 script, u32 *arguments,
                       s32 sound, s32 persistent, s32 option)
{
    struct TriggerManager *manager = TriggerManager_GetOrCreate();
    struct Trigger *p;
    s32 i;
    u32 *in, *out;
    if (!manager)
        return -1;
    p = Heap_Alloc(188);
    if (!p)
        return -1;
    Memory_Clear(p, 188);
    p->id = id;
    p->kind = kind;
    p->actor = -1;
    p->type = 0;
    p->flags = 0;
    p->flip = 0;
    p->timer = 0;
    p->sprite = 0;
    p->object = 0;
    p->other = 0;
    p->animation = 0;
    p->update = Trigger_UpdateFalling;
    p->tick = 0;
    p->option = option;
    p->script = script;
    in = arguments;
    out = p->arguments;
    i = 2;
    do {
        *out++ = *in++;
    } while (--i >= 0);
    p->sprite = Heap_Alloc(40);
    if (!p->sprite) {
        Heap_Free(p);
        return -1;
    }
    Memory_Clear(p->sprite, 40);
    ParticleSprite_Initialize(p->sprite, manager->graphics, 0);
    Effect_SetOrigin(p->sprite, -8, -10);
    if (Terrain_HasHigherCorner(position, 128)) {
        {
            struct TriggerSprite *sprite = p->sprite;
            struct TriggerGraphics *graphics = manager->graphics;
            sprite->tile = ((sprite->width >> 3) * (sprite->height >> 3)) * 35 + graphics->base;
        }
        p->sprite->mode = 1;
    } else {
        {
            struct TriggerSprite *sprite = p->sprite;
            struct TriggerGraphics *graphics = manager->graphics;
            sprite->tile = ((sprite->width >> 3) * (sprite->height >> 3)) * 32 + graphics->base;
        }
        p->sprite->mode = 2;
    }
    p->sprite->position = *position;
    p->sprite->priority = 16;
    p->position = &p->sprite->position;
    if (sound)
        p->sound = Region_FindContaining(p->position, sound);
    else
        p->sound = sound;
    p->persistent = persistent;
    ScaledShadow_Initialize(p->effect, &p->sprite->position, 64, 24, 40, 32, 512, 1, 5, 1, 0);
    TriggerManager_InsertTrigger(p);
    return 0;
}

s32 Trigger_CreateDropFromScript(void)
{
    s32 arguments[3];
    struct SceneVec position;
    s32 script, id, kind, mode, persistent, option;
    s32 *out, count;
    if (Script_SeekToKeyword('s')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    if (Script_SeekToKeyword('I')) {
        id = Script_GetValue();
        kind = Script_GetValue();
    } else {
        id = 0;
        kind = 0;
    }
    script = Script_GetKeywordValue('p', 0);
    mode = Script_GetKeywordValue('M', 0);
    persistent = Script_GetKeywordValue('i', 0);
    option = Script_GetKeywordValue('O', 0);
    {
        s32 zero = 0;
        s32 *clear = arguments + 2;
        do {
            *clear-- = zero;
        } while ((s32)clear >= (s32)arguments);
    }
    if (Script_SeekToKeyword('a')) {
        count = 0;
        out = arguments;
        while (Script_GetPc() && count < 3) {
            *out++ = Script_GetValue();
            count++;
        }
    }
    return Trigger_CreateDrop(&position, id, kind, script, arguments, mode, persistent, option);
}

s32 Trigger_CreateContainer(struct SceneVec *position, s32 id, s32 kind, s32 flip, s32 bob,
                            u32 startScript, u32 script, u32 *arguments, s32 sound, s32 hidden,
                            s32 condition)
{
    struct TriggerManager *manager = TriggerManager_GetOrCreate();
    struct Trigger *p;
    s32 i;
    u32 *in, *out;
    if (!manager)
        return -1;
    p = Heap_Alloc(188);
    if (!p)
        return -1;
    Memory_Clear(p, 188);
    p->id = id;
    p->kind = kind;
    p->actor = -1;
    p->type = 1;
    p->flags = 0;
    p->flip = flip;
    p->timer = 0;
    p->sprite = 0;
    p->object = 0;
    p->other = 0;
    p->animation = 0;
    if (bob) {
        p->update = Trigger_UpdateBobbingAnimation;
        if (!(gEngineFlags & 2048))
            Sound_PlaySong(623);
    } else
        p->update = Trigger_UpdateIdleAnimation;
    p->startScript = startScript;
    p->script = script;
    in = arguments;
    out = p->arguments;
    i = 2;
    do {
        *out++ = *in++;
    } while (--i >= 0);
    p->object = Heap_Alloc(44);
    if (!p->object) {
        Heap_Free(p);
        return -1;
    }
    Memory_Clear(p->object, 44);
    p->other = Heap_Alloc(28);
    if (!p->other) {
        if (p->object)
            Heap_Free(p->object);
        Heap_Free(p);
        return -1;
    }
    Memory_Clear(p->other, 28);
    p->animation = Heap_Alloc(16);
    if (!p->animation) {
        if (p->object)
            Heap_Free(p->object);
        if (p->other)
            Heap_Free(p->other);
        Heap_Free(p);
        return -1;
    }
    Memory_Clear(p->animation, 16);
    if (!AnimData_Load(p->other, manager->kind)) {
        if (p->object)
            Heap_Free(p->object);
        if (p->other)
            Heap_Free(p->other);
        if (p->animation)
            Heap_Free(p->animation);
        Heap_Free(p);
        return -1;
    }
    Sprite_Init(p->object, p->other, 0);
    p->object->position = *position;
    p->object->position.y = groundHeight((struct SceneVec *)&p->object->position);
    if (p->flip)
        p->object->flags |= 4;
    p->position = &p->object->position;
    p->started = hidden;
    p->condition = condition;
    if ((u8)condition == 0)
        AnimData_SelectPaletteBank(p->other, 772);
    else if ((u8)condition == 1)
        AnimData_SelectPaletteBank(p->other, 775);
    else
        AnimData_SelectPaletteBank(p->other, 776);
    {
        s32 hidden = p->started;
        if (hidden) {
            s32 available;
            p->object->flags |= 1;
            if (gEngineFlags & 2048) {
                available = stateMode();
            } else {
                if (gUnk_02000478 > 0)
                    goto reveal;
                if (gUnk_02000580[0] && (({
                        u32 *flags = &gUnk_02000580[0]->actionFlags;
                        u32 mask = 16;
                        *flags &mask;
                    })))
                    goto reveal;
                available = globalMode();
            }
            asm("" : : "r"(available));
            if (!available)
                goto conceal;
        reveal:
            hidden = 0;
            goto visibilityDone;
        conceal:
            hidden = 1;
        }
    visibilityDone:
        p->visible = hidden;
    }
    {
        void *animation = p->animation;
        void *resource = ({
            struct TriggerManager *m = manager;
            m->resource;
        });
        register s32 zero asm("r2") = 0;
        Anim_SetSequence(animation, resource, zero, 0, zero);
    }
    {
        s32 value;
        if (({
                register s32 input asm("r3") = sound;
                input;
            }))
            value = Region_FindContaining(&p->object->position);
        else
            value = *(u16 *)&sound;
        p->sound = value;
    }
    ScaledShadow_Initialize(p->effect, &p->object->position, 64, 24, 40, 32, 512, 1, 5, 1, ({
                                register s32 zero asm("r0") = 0;
                                zero;
                            }));
    ScaledShadow_Disable(p->effect);
    TriggerManager_InsertTrigger(p);
    return 0;
}

s32 Trigger_CreateContainerFromScript(void)
{
    s32 arguments[3];
    struct SceneVec position;
    s32 flip, id, kind, startScript, script, bob, sound, hidden, condition;
    s32 *out, count;
    if (Script_SeekToKeyword('s')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    if (Script_SeekToKeyword('I')) {
        id = Script_GetValue();
        kind = Script_GetValue();
    } else {
        id = 0;
        kind = 0;
    }
    flip = Script_GetKeywordValue('d', 0);
    startScript = Script_GetKeywordValue('g', 0);
    script = Script_GetKeywordValue('p', 0);
    bob = Script_GetKeywordValue('m', 0);
    sound = Script_GetKeywordValue('M', 0);
    hidden = Script_GetKeywordValue('S', 0);
    condition = Script_GetKeywordValue('c', 0);
    {
        s32 zero = 0;
        s32 *clear = arguments + 2;
        do {
            *clear-- = zero;
        } while ((s32)clear >= (s32)arguments);
    }
    if (Script_SeekToKeyword('a')) {
        count = 0;
        out = arguments;
        while (Script_GetPc() && count < 3) {
            *out++ = Script_GetValue();
            count++;
        }
    }
    return Trigger_CreateContainer(&position, id, kind, flip, bob, startScript, script, arguments,
                                   sound, hidden, condition);
}

s32 TriggerManager_DestroyAllOfType(u32 type)
{
    struct TriggerManager *s = TriggerManager_GetOrCreate();
    struct Trigger *n;
    if (s == 0)
        return -1;
    n = s->head;
    while (n != 0) {
        struct Trigger *next = n->next;
        if (n->type == type)
            Trigger_Destroy(n);
        n = next;
    }
}
u32 TriggerManager_DestroyAllOfTypeFromScript(void)
{
    return TriggerManager_DestroyAllOfType(Script_GetKeywordValue('t', 0));
}

s32 Trigger_MoveDropsByItem(struct SceneVec *position, s32 id, s32 kind)
{
    struct TriggerManager *manager = TriggerManager_GetOrCreate();
    struct Trigger *p, *next;
    if (!manager)
        return -1;
    for (p = manager->head; p; p = next) {
        next = p->next;
        if (p->type == 0 && p->option && p->id == id && p->kind == kind &&
            p->update != Trigger_UpdateDropTimer)
            *p->position = *position;
    }
}

u32 Trigger_MoveDropsByItemFromScript(void)
{
    struct {
        u32 a : 16;
        u32 b : 16;
        u32 c : 16;
    } bf;
    u32 x = Script_GetKeywordValue(0x6b, 0);
    u32 y = Script_GetKeywordValue(0x69, 0);
    if (Script_SeekToKeyword(0x70)) {
        bf.a = Script_GetValue();
        bf.b = Script_GetValue();
        bf.c = Script_GetValue();
    }
    return Trigger_MoveDropsByItem((struct SceneVec *)&bf, x, y);
}
