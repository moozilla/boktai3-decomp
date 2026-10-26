#include "collision_controller.h"
/* CollisionController_ClearGlobal: Zeroes gCollisionManager; matches the ClearGlobal pattern already used by EffectManager_ClearGlobal and is hinted by the file's own #define. */
#define CollisionController_ClearGlobal sub_08009E30
// MATCHING_CONCESSION: register-pinning

#define CollisionController_ClearGlobal sub_08009E30
void CollisionController_ClearGlobal(void)
{
    gCollisionManager = 0;
}

struct CollisionSlot *CollisionController_FindFreeSlot(struct CollisionManager *manager)
{
    s32 i = 0;
    struct CollisionSlot *slot = manager->slots;
    while (i <= 15) {
        if (slot->active == 0) {
            return slot;
        }
        slot++;
        i++;
    }
    return 0;
}

void CollisionController_ReceiveHit(struct SceneCollider *collider, struct CollisionOther *other,
                                    struct CollisionSlot *slot)
{
    u32 m = 0x4000;
    if ((collider->contactFlags2 & m) == 0) {
        u32 m2 = 0xe00;
        u32 x = collider->contactFlags & m2;
        if (x == 0) {
            other->hit = 1;
            slot->mode = 1;
            slot->timer = x;
            slot->entered = 1;
            slot->collider.state |= 4;
        }
    }
}

s32 CollisionController_InitSlot(struct CollisionManager *manager, struct CollisionSlot *slot)
{
    struct SceneExtent scale, offset;
    void *collider, *sprite;
    scale.x = 30;
    scale.y = 320;
    scale.z = 30;
    offset.x = 0;
    offset.y = 160;
    offset.z = 0;
    collider = &slot->collider;
    Collider_Init(collider, 0, 0x4020, 0, 16, &scale, &offset);
    Collider_SetCallback(collider, CollisionController_ReceiveHit, slot);
    Collider_Refresh(collider);
    sprite = &slot->sprite;
    ParticleSprite_Initialize(sprite, manager->graphics, 1);
    SpriteDrawable_SelectTileFrame(sprite, manager->graphics, 12);
    Effect_SetOrigin(sprite, -8, -8);
    slot->active = 1;
}

u32 CollisionController_RemoveSlot(struct CollisionManager *manager, struct CollisionSlot *slot)
{
    Effect_Remove(&slot->sprite);
    Collider_Remove(&slot->collider);
    return slot->active = 0;
}

void CollisionController_UpdateIdleAnimationPaused(u32 x, struct CollisionSlot *slot)
{
    if (slot->entered != 0) {
        slot->entered = 0;
        slot->sprite.flags |= 1;
    }
}

static inline s32 stateMode(void)
{
    struct Player *a = gUnk_02000580[0];
    if (a) {
        u32 active = a->active;
        s32 yes = 0;
        if (active && a->sceneMode == 4)
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

static inline void resetIdle(struct CollisionManager *manager)
{
    u32 one = 1;
    u8 *p = &manager->slots[0].mode;
    register s32 i asm("r6") = 15;
    do {
        if (p[0] == 0) {
            p[0] = 0;
            *(u16 *)(p + 8) = 0;
            p[5] = one;
        }
        p += 160;
    } while (--i >= 0);
}

static inline u32 randomDelay(u32 restart)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * 2;
        u32 value;
        if (restart) {
            register u16 *table asm("r2") = gUnk_0203B400;
            value = *(u16 *)(offset + (u32)table);
        } else
            value = *(u16 *)((u8 *)gUnk_0203B400 + offset);
        value &= 127;
        value += 120;
        return value;
    }
}
void CollisionController_UpdateIdleAnimation(struct CollisionManager *manager,
                                             struct CollisionSlot *p)
{
    struct CollisionSprite *o;
    struct CollisionAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    if (p->entered) {
        p->entered = 0;
        Effect_SetOrigin(&p->sprite, -4, -4);
        SpriteDrawable_SelectTileFrame(&p->sprite, manager->animationGraphics, 0);
        Anim_SetSequence(&p->anim, manager->idleResource, 8, 0, 0);
        p->sprite.flags &= ~1;
        p->delay = randomDelay(0);
    }
    {
        u32 flags = p->sprite.flags;
        register u32 low asm("r2") = 1;
        u32 firstMask;
        firstMask = low;
        if (flags & low) {
            if (p->timer++ > p->delay) {
                p->timer = 0;
                p->delay = randomDelay(1);
                Effect_SetOrigin(&p->sprite, -4, -4);
                SpriteDrawable_SelectTileFrame(&p->sprite, manager->animationGraphics, 0);
                Anim_SetSequence(&p->anim, manager->idleResource, 8, 0, 0);
                p->sprite.flags &= ~1;
            }
        } else {
            o = &p->sprite;
            {
                struct CollisionGraphics *graphics = manager->animationGraphics;
                a = &p->anim;
                entry = a->data + a->frame;
                {
                    u32 factor = *entry >> 6;
                    o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
                }
            }
            {
                u32 input = a->flags;
                register u32 left asm("r1") = firstMask;
                left &= input;
                if (left != (((*entry & 0x30) >> 4) & firstMask))
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
                a->rate = ((*entry & 15) * a->scale) >> 6;
            } else
                done = 0;
            p->done = done;
            if (done)
                p->sprite.flags |= 1;
        }
    }
}

void CollisionController_UpdateHitDelayAnimation(u32 x, struct CollisionSlot *slot)
{
    u16 n;
    if (slot->entered != 0)
        slot->entered = 0;
    n = slot->timer++;
    if (n > 0x1e) {
        u32 t = 2;
        slot->mode = t;
        slot->timer = 0;
        slot->entered = 1;
    }
}

void CollisionController_UpdateHitAnimation(struct CollisionManager *manager,
                                            struct CollisionSlot *p)
{
    struct CollisionSprite *o;
    struct CollisionAnim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    struct SceneExtent effect;
    if (p->entered) {
        p->entered = 0;
        p->sprite.flags &= ~1;
        Effect_SetOrigin(&p->sprite, -8, -8);
        SpriteDrawable_SelectTileFrame(&p->sprite, manager->graphics, 0);
        Anim_SetSequence(&p->anim, manager->hitResource, 0, 0, 0);
        Sound_PlaySongChecked(290);
    }
    if (p->anim.frame == 1 && p->anim.timer == 0)
        p->sprite.position.y += 16;
    if (p->anim.frame == 2 && p->anim.timer == 0)
        p->sprite.position.y -= 16;
    if (p->anim.frame == 4 && p->anim.timer == 0) {
        effect.x = 10;
        effect.y = 10;
        effect.z = 10;
        EffectManager_SpawnParticles(p->kind, p->type | 16, p->attack, &p->sprite.position, &effect,
                                     p->range, p->script);
        if (p->type == 2)
            Sound_PlaySongChecked(696);
        else
            Sound_PlaySongChecked(285);
    }
    o = &p->sprite;
    {
        struct CollisionGraphics *graphics = manager->graphics;
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
        a->rate = ((*entry & 15) * a->scale) >> 6;
    } else
        done = 0;
    p->done = done;
    if (done) {
        p->finished = 1;
        p->sprite.flags |= 1;
    }
}

s32 CollisionController_UpdateAll(struct CollisionManager *input)
{
    struct CollisionManager *manager = input;
    s32 paused = 0;
    s32 i;
    struct CollisionSlot *p;
    u32 mode = gUnk_03005430.mode;
    s32 available;
    if ((u8)(mode - 4) <= 1 || mode == 0)
        goto pause;
    if (gEngineFlags & 2048)
        available = stateMode();
    else {
        struct Player *actor;
        if (gUnk_02000478 > 0)
            goto pause;
        actor = gUnk_02000580[0];
        if (actor) {
            u32 *address = &actor->actionFlags;
            u32 mask = 16;
            if (*address & mask)
                goto pause;
        }
        available = globalMode();
    }
    asm("" : : "r"(available));
    if (available) {
    pause:
        paused = 1;
        if (manager->paused == 0)
            resetIdle(manager);
    } else if (manager->paused == 1)
        resetIdle(manager);
    manager->paused = paused;
    asm("" : "+r"(manager) : : "r4", "r5", "r6");
    {
        register void **table asm("r9") = &gUnk_08604A00[0][0];
        u32 row = paused * 12;
        u8 *control = &manager->slots[0].mode;
        p = manager->slots;
        for (i = 15; i >= 0; control += 160, p++, i--)
            if (control[3]) {
                if (control[4])
                    CollisionController_RemoveSlot(manager, p);
                else {
                    control[6] = 0;
                    sub_08249240(manager, p, *(void **)(control[0] * 4 + row + (u32)table));
                }
            }
    }
    return 0;
}

s32 CollisionController_DestroyManager(struct CollisionManager *manager)
{
    struct CollisionSlot *slot = manager->slots;
    s32 i;
    for (i = 15; i >= 0; i--) {
        if (slot->active != 0)
            CollisionController_RemoveSlot(manager, slot);
        slot++;
    }
    {
        u32 z = 0;
        gCollisionManager = (struct CollisionManager *)z;
    }
    return 0;
}

s32 CollisionController_InitManager(struct CollisionManager *manager, void *owner)
{
    u32 mode;
    s32 available;
    gCollisionManager = manager;
    mode = gUnk_03005430.mode;
    if ((((u8)(mode - 4)) <= 1) || (mode == 0)) {
        goto pause;
    }
    if (gEngineFlags & 2048) {
        *(&available) = stateMode();
    } else {
        struct Player *actor;
        if (gUnk_02000478 > 0) {
            goto pause;
        }
        actor = gUnk_02000580[0];
        if (actor) {
            u32 *address = &actor->actionFlags;
            u32 mask = 16;
            if ((*address) & mask) {
                goto pause;
            }
        }
        available = globalMode();
    }
    if (available) {
    pause:
        available = 1;
    }
    manager->paused = available;
    manager->graphics = AnimData_FindEntry(0x1c1a);
    manager->animationGraphics = AnimData_FindEntry(0x1c1c);
    manager->hitResource = ScriptProc_Start(0x922e, 0xd1b8);
    manager->idleResource = ScriptProc_Start(0x922e, 0xd41f);
    return 0;
}

struct CollisionManager *CollisionController_CreateManager(void *owner, u32 unused)
{
    struct CollisionManager *p = Task_Create(10, 0xA2C);
    if (p != 0) {
        Task_SetCallbacks(p, CollisionController_UpdateAll, CollisionController_DestroyManager);
        if (CollisionController_InitManager(p, owner) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

s32 CollisionController_ScriptCreate(void)
{
    struct SceneExtent position;
    struct CollisionManager *manager = gCollisionManager;
    struct CollisionSlot *p;
    if (!manager) {
        manager = CollisionController_CreateManager(0, 0);
        if (!manager)
            goto fail;
    }
    p = CollisionController_FindFreeSlot(manager);
    if (!p) {
    fail:
        return -1;
    }
    Memory_Clear(p, 160);
    p->id = Script_GetKeywordValue('n', 0);
    p->type = Script_GetKeywordValue('t', 0);
    p->kind = Script_GetKeywordValue('b', 0);
    p->done = 0;
    p->mode = 0;
    p->timer = 0;
    p->entered = 1;
    p->delay = 0;
    p->finished = 0;
    CollisionController_InitSlot(manager, p);
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    Collider_SetPosition(&p->collider, (struct SceneVec *)&position, 0);
    p->sprite.position = *(struct CollisionVec *)&position;
    p->sprite.position.y += 230;
    if (Script_SeekToKeyword('s')) {
        p->attack = Script_GetValue();
        p->range = Script_GetValue();
        p->script = Script_GetValue();
    } else {
        p->attack = 50;
        p->range = 600;
        p->script = 100;
    }
    return 0;
}

s32 CollisionController_ScriptGetActiveCount(void)
{
    s32 count;
    struct CollisionManager *manager;
    s32 i;
    struct CollisionSlot *slot;
    manager = gCollisionManager;
    count = 0;
    if (manager != 0) {
        slot = manager->slots;
        for (i = 0xF; i >= 0; i--) {
            if (slot->active != 0)
                count++;
            slot++;
        }
    }
    {
        u8 temp[8];
        Script_ReadReference(temp);
        Script_WriteReferenceValue(temp, 0, count);
    }
    return count;
}

void CollisionController_ScriptRemoveAll(void)
{
    struct CollisionManager *manager = gCollisionManager;
    s32 i;
    struct CollisionSlot *slot;
    if (manager == 0)
        return;
    slot = manager->slots;
    for (i = 0xF; i >= 0; i--) {
        if (slot->active != 0)
            CollisionController_RemoveSlot(manager, slot);
        slot++;
    }
}
