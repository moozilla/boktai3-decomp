#include "falling_objects.h"
/* FallingObjectManager_ClearGlobal: clears the global FallingObjectManager pointer (gFallingObjectManager); already aliased this way in src/falling_objects.c */
#define FallingObjectManager_ClearGlobal sub_0800CBA0

#define FallingObjectManager_ClearGlobal sub_0800CBA0

void FallingObjectManager_ClearGlobal(void)
{
    gFallingObjectManager = 0;
}

void FallingObject_ReceiveHit(struct FallingHit *a, u32 unused, struct FallingObject *b)
{
    struct FallingObject *c = b;
    if (c->type == 0) {
        u32 fl = a->flags;
        u32 d;
        d = a->damage;
        if (fl & 8) {
            d <<= 1;
            b->health -= d;
        }
        if (c->health < 0)
            c->health = 0;
    }
}

void FallingObject_ReceiveAttackHit(void)
{
}

u32 FallingObjectManager_AddObject(struct FallingObjectManager *head, struct FallingObject *n)
{
    struct FallingObject **slot;
    struct FallingObject *t;
    n->previous = 0;
    slot = &n->next;
    *slot = head->head;
    t = *slot;
    if (t) {
        t->previous = n;
    }
    head->head = n;
    return 0;
}

u32 FallingObjectManager_RemoveObject(struct FallingObjectManager *head, struct FallingObject *n)
{
    struct FallingObject *prev = n->previous;
    struct FallingObject *next = n->next;
    if (prev) {
        prev->next = next;
    } else {
        head->head = next;
    }
    if (next) {
        next->previous = prev;
    }
    return 0;
}

void ScaledShadow_Destroy(void *effect);

void FallingObject_ClearEffect(void *object)
{
    struct FallingObject *p = object;
    if (p->effectActive) {
        ScaledShadow_Destroy(p->effect);
        p->effectActive = 0;
    }
}

u32 FallingObject_Destroy(struct FallingObject *s)
{
    Sprite_Remove(&s->sprite);
    if (gFallingObjectManager != 0)
        FallingObjectManager_RemoveObject(gFallingObjectManager, s);
    FallingObject_ClearEffect(s);
    if (s->collider != 0) {
        Collider_Remove(s->collider);
        Heap_Free(s->collider);
    }
    if (s->attack != 0)
        Heap_Free(s->attack);
    Heap_Free(s);
}

struct Cell {
    u8 height, tile;
    u16 flags;
};

extern u16 gUnk_08604ABC[];

u8 *TerrainModifiers_FindCellOverride(s32, s32);

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

void ScaledShadow_Initialize(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32);

static inline s32 cellIndex(struct FallingVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline struct Cell *getCell(s32 index)
{
    u8 *dynamic = TerrainModifiers_FindCellOverride(index, 1);
    if (dynamic)
        return (struct Cell *)(dynamic + 4);
    return (struct Cell *)(gTerrainMap->cells + 12 + index * 4);
}

s32 FallingObject_Create(u16 id, s32 type, u32 flags, u32 parameter, u32 speed, u32 health,
                         u32 power, u32 force, struct FallingVec *position)
{
    struct FallingPackedVec size, offset;
    struct FallingObject *p;
    s32 variant;
    struct FallingVec *pos;
    void *resource;
    if (!gFallingObjectManager && !FallingObjectManager_Create((void *)0xc0db, 0))
        return -1;
    p = Heap_Alloc(sizeof(*p));
    if (!p)
        return -1;
    Memory_Clear(p, sizeof(*p));
    p->id = id;
    variant = 0;
    if (type > 1) {
        p->type = 2;
        variant = type - 2;
    } else
        p->type = type;
    p->state = 0;
    p->timer = 0;
    {
        u32 one = 1;
        p->entered = one;
        p->flags = flags;
        p->parameter = parameter;
        p->position = *position;
        if (p->type == 0)
            p->animated = one;
        else
            p->animated = 0;
    }
    p->speed = speed;
    {
        struct FallingVec *groundPos = &p->position;
        s32 x = p->position.x >> 8, z = groundPos->z >> 8, index;
        struct Cell *cell;
        u32 slope;
        s32 height;
        pos = groundPos;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        cell = getCell(index);
        slope = cell->height >> 4;
        height = (cell->height & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)groundPos->z;
                break;
            case 2:
                height -= (u8)groundPos->x;
                break;
        }
        p->ground = height;
    }
    p->health = health;
    if (p->flags & 1) {
        p->collider = Heap_Alloc(84);
        if (!p->collider)
            return -1;
        Memory_Clear(p->collider, 84);
        if (p->flags & 4) {
            size.x = 180;
            size.y = 256;
            size.z = 180;
            offset.x = 0;
            offset.y = 256;
            offset.z = 0;
        } else {
            size.x = 90;
            size.y = 128;
            size.z = 90;
            offset.x = 0;
            offset.y = 128;
            offset.z = 0;
        }
        Collider_Init(p->collider, p->id, 0x4001, 0, 16, &size, &offset);
        Collider_SetCallback(p->collider, FallingObject_ReceiveHit, p);
        Collider_Refresh(p->collider);
        Collider_SetPosition(p->collider, pos, 0);
    } else
        p->collider = 0;
    if ((u8)(p->flags & 2)) {
        u32 attackFlags;
        u32 big;
        p->attack = Heap_Alloc(84);
        if (!p->attack)
            return -1;
        Memory_Clear(p->attack, 84);
        big = (u8)(p->flags & 4);
        if (big) {
            big = 0;
            ((struct FallingVec *)&size)->x = 120;
            ((struct FallingVec *)&size)->y = 160;
            ((struct FallingVec *)&size)->z = 120;
            ((struct FallingVec *)&offset)->x = big;
            ((struct FallingVec *)&offset)->y = 80;
            ((struct FallingVec *)&offset)->z = big;
        } else {
            ((struct FallingVec *)&size)->x = 60;
            ((struct FallingVec *)&size)->y = 80;
            ((struct FallingVec *)&size)->z = 60;
            ((struct FallingVec *)&offset)->x = big;
            ((struct FallingVec *)&offset)->y = 40;
            ((struct FallingVec *)&offset)->z = big;
        }
        attackFlags = 0;
        if (!p->type)
            attackFlags = 0x200004;
        Collider_Init(p->attack, p->id, 0x2001, 0, 32, &size, &offset);
        Collider_SetAttackProperties(p->attack, power, force, 65536, attackFlags, 60);
        Collider_SetCallback(p->attack, FallingObject_ReceiveAttackHit, p);
        Collider_SetPosition(p->attack, pos, 0);
    } else
        p->attack = 0;
    resource = p->resource;
    if (!AnimData_Load(resource, gUnk_08604ABC[p->type]))
        return -1;
    Sprite_Init(&p->sprite, resource, 0);
    p->sprite.priority = 2;
    p->sprite.position = p->position;
    if (type > 1)
        AnimData_SelectPaletteBank(resource, 739 + variant);
    if (p->flags & 4) {
        p->sprite.flags |= 2;
        p->sprite.scaleY = 120;
        p->sprite.scaleX = 120;
    } else {
        p->sprite.flags &= ~2;
        p->sprite.scaleY = 64;
        p->sprite.scaleX = 64;
    }
    if (p->animated)
        Anim_SetSequence(&p->anim, gFallingObjectManager->animation, 3, 0, 0);
    ScaledShadow_Initialize(p->effect, pos, 80, 7, 107, 107, 3200, 1, 5, 1, 0);
    p->effectActive = 1;
    p->remove = 0;
    p->previous = 0;
    p->next = 0;
    return FallingObjectManager_AddObject(gFallingObjectManager, p);
}

s32 FallingObject_CreateFromScript(void)
{
    struct FallingPackedVec position;
    u16 id;
    s32 type;
    u32 flags, parameter, speed, health, power, force;
    id = (u16)Script_GetKeywordValue(110, 0);
    type = Script_GetKeywordValue(116, 0);
    flags = Script_GetKeywordValue(102, 0);
    parameter = Script_GetKeywordValue(84, 0);
    speed = Script_GetKeywordValue(115, 0);
    health = Script_GetKeywordValue(108, 0);
    power = Script_GetKeywordValue(80, 200);
    force = Script_GetKeywordValue(70, 50);
    if (Script_SeekToKeyword(112)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    return FallingObject_Create(id, type, flags, parameter, speed, health, power, force,
                                (struct FallingVec *)&position);
}

void FallingObject_UpdateFalling(struct FallingObject *object)
{
    if (object->entered) {
        object->entered = 0;
        if (object->animated)
            Anim_SetSequence(&object->anim, gFallingObjectManager->animation, 3, 0, 0);
    }
    object->position.y -= object->speed;
    if (object->position.y <= object->ground) {
        object->position.y = object->ground;
        if (object->type == 2) {
            if (object->flags & 4)
                Sound_PlaySong(669);
            else
                Sound_PlaySong(511);
        } else
            Sound_PlaySong(326);
        {
            u32 one = 1;
            u32 zero = 0;
            u32 frame;
            object->state = one;
            frame = 0;
            object->timer = zero;
            object->entered = one;
            object->frame = frame;
            object->frameTimer = frame;
        }
    }
    if (object->type > 1) {
        object->frameTimer++;
        if (object->frameTimer == 5) {
            object->frame++;
            if (object->frame > 2)
                object->frame = 0;
            object->frameTimer = 0;
        }
        FallingObject_SetFrame(&object->sprite, object->frame);
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
}

void FadingParticleGroup_Spawn(u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);

void FallingObject_UpdateLanding(struct FallingObject *object)
{
    if (object->entered) {
        object->entered = 0;
        Anim_SetSequence(&object->anim, gFallingObjectManager->animation, 2, 0, 0);
        FallingObject_ClearEffect(object);
        FadingParticleGroup_Spawn((FallingObject_Random() & 3) + 6, &object->position, 9, 20, 8, 8, 0, 256, 24,
                     16);
    }
    if (object->type > 1) {
        object->frameTimer++;
        if (object->frameTimer == 10) {
            object->frame++;
            if (object->frame > 2)
                object->frame = 0;
            object->frameTimer = 0;
        }
        FallingObject_SetFrame(&object->sprite, object->frame);
    }
    if (object->health <= 0) {
        FallingObject_SetState(object, 3);
        return;
    }
    if (object->timer++ > 30) {
        FallingObject_SetState(object, 2);
    } else {
        s32 amount = (30 - object->timer) >> 1;
        if (!(object->timer & 1)) {
            object->sprite.position.x += amount;
            object->sprite.position.z -= amount;
        } else {
            object->sprite.position.x -= amount;
            object->sprite.position.z += amount;
        }
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
}

void sub_08014634(u32, u32, void *, void *, void *, u32, u32);

void Camera_RequestShake(u32);

void FallingObject_UpdateShatter(struct FallingObject *object)
{
    struct FallingPackedVec low, high;
    if (object->entered) {
        object->entered = 0;
        FallingObject_ClearEffect(object);
        if (!(gEngineFlags & 2048)) {
            low.x = -15;
            low.y = 8;
            low.z = -15;
            high.x = 30;
            high.y = 22;
            high.z = 30;
            sub_08014634(8, 3, &object->position, &low, &high, 60, 60);
        }
        object->parameter = 30;
    }
    if (object->timer < 12) {
        object->position.y += 8;
        Camera_RequestShake(40);
    } else {
        object->position.y -= 4;
        if (object->position.y < object->ground)
            object->position.y = object->ground;
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
    if ((object->timer & 7) < 2)
        object->sprite.flags |= 1;
    else
        object->sprite.flags &= ~1;
    if (object->timer++ > object->parameter)
        object->remove = 1;
}

void FallingObject_UpdateBounceAway(struct FallingObject *object)
{
    struct FallingPackedVec low, high;
    if (object->entered) {
        object->entered = 0;
        if (!(gEngineFlags & 2048)) {
            low.x = -15;
            low.y = 8;
            low.z = -15;
            high.x = 30;
            high.y = 22;
            high.z = 30;
            sub_08014634(8, 3, &object->position, &low, &high, 60, 60);
        }
        object->parameter = 25;
    }
    if (object->timer < 15) {
        object->position.y -= (object->timer >> 2) - 16;
        Camera_RequestShake(40);
    } else {
        s32 amount = object->timer - 15;
        if (amount < 16)
            object->position.y -= amount;
        else
            object->position.y -= 16;
        if (object->position.y < object->ground)
            object->position.y = object->ground;
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
    if ((object->timer & 7) < 2)
        object->sprite.flags |= 1;
    else
        object->sprite.flags &= ~1;
    if (object->timer++ > object->parameter) {
        FallingObject_ClearEffect(object);
        object->remove = 1;
    }
}

void FallingObject_UpdateWaiting(struct FallingObject *object)
{
    if (object->entered) {
        object->entered = 0;
        Anim_SetSequence(&object->anim, gFallingObjectManager->animation, 2, 0, 0);
    }
    if (object->health <= 0 || object->timer++ > object->parameter) {
        FallingObject_SetState(object, 3);
        return;
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
}

void FallingObject_ClearEffect(void *);

s32 AnimatedParticles_EmitBurst(void *, u32, u32, u32, u32, u32, u32, u32);

s32 FallingObject_SpawnFinishParticles(struct FallingObject *object)
{
    struct FallingPackedVec position;
    position.x = object->sprite.position.x;
    position.y = object->sprite.position.y;
    position.z = object->sprite.position.z;
    return AnimatedParticles_EmitBurst(&position, 2, 0, 64, 8, 4, 0, 0);
}

void FallingObject_UpdateFinish(struct FallingObject *object)
{
    if (object->entered) {
        object->entered = 0;
        Anim_SetSequence(&object->anim, gFallingObjectManager->animation, 4, 0, 0);
        if (object->collider) {
            u32 flags = ((u16 *)object->collider)[3];
            u32 result;
            result = 4;
            result |= flags;
            ((u16 *)object->collider)[3] = result;
        }
        FallingObject_ClearEffect(object);
    }
    if (object->animationDone) {
        if (!object->remove)
            FallingObject_SpawnFinishParticles(object);
        object->remove = 1;
    }
}

// MATCHING_CONCESSION: register-pinning
s32 FallingObjectManager_Update(struct FallingObjectManager *manager)
{
    struct FallingObject *p = manager->head, *next;
    struct FallingSprite *o;
    struct FallingAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    u32 one;
    if (p) {
        u32 initial = 1;
        one = initial;
    }
    for (; p; p = next) {
        next = p->next;
        if (p->remove) {
            FallingObject_Destroy(p);
            continue;
        }
        if (p->animated) {
            o = &p->sprite;
            a = &p->anim;
            entry = a->data + a->frame;
            o->frame = *entry >> 6;
            {
                u32 input = a->flags;
                register u32 left asm("r2") = (u8)(one & input);
                if (left != (((*entry & 0x30) >> 4) & one))
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
                {
                    register u32 frame asm("r1") = *entry;
                    a->factor = frame & 15;
                }
                a->rate = (a->factor * a->scale) >> 6;
                if (!a->rate)
                    a->rate = one;
            } else
                done = 0;
            p->animationDone = done;
        }
        p->sprite.position = p->position;
        gUnk_08604ACC[p->type][p->state](p);
    }
    return 0;
}

u32 FallingObjectManager_Destroy(struct FallingObjectManager *s)
{
    struct FallingObject *n = s->head;
    while (n != 0) {
        struct FallingObject *next = n->next;
        FallingObject_Destroy(n);
        n = next;
    }
    return (u32)(gFallingObjectManager = 0);
}

s32 FallingObjectManager_Initialize(struct FallingObjectManager *p, void *unused)
{
    gFallingObjectManager = p;
    p->animation = ScriptProc_Start(0x922E, 0x931E);
    p->head = 0;
    return 0;
}

void *FallingObjectManager_Create(void *a, u32 unused)
{
    struct FallingObjectManager *p;
    if (gFallingObjectManager != 0)
        return gFallingObjectManager;
    p = Task_Create(8, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, (void *)FallingObjectManager_Update,
                          (void *)FallingObjectManager_Destroy);
        if (FallingObjectManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
