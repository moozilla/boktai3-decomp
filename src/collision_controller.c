#include "collision_controller.h"
#include "scene_actor_status.h"
#include "sprite_effect_controller.h"
#include "sun_clock.h"
#include "player_slot_status.h"

static inline void CollisionController_SetManager(struct CollisionManager *manager)
{
    gCollisionManager = manager;
}

void CollisionController_ClearGlobal(void)
{
    gCollisionManager = NULL;
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

static inline void CollisionController_SetMode(struct CollisionSlot *slot, u32 mode)
{
    slot->mode = mode;
    slot->timer = 0;
    slot->entered = 1;
}

void CollisionController_ReceiveHit(struct SceneCollider *collider, struct CollisionOther *other,
                                    struct CollisionSlot *slot)
{
    if (SceneCollider_ReadReactionFlags(collider, 0x4000) == 0) {
        u32 attackBits = SceneCollider_ReadContactFlags(collider, 0xe00);
        if (attackBits == 0) {
            other->hit = 1;
            CollisionController_SetMode(slot, 1);
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

static inline u8 stateMode(void)
{
    struct Player *a = gPlayers[0];
    if (a) {
        u32 active = a->active;
        s32 yes = 0;
        if (active && a->sceneMode == 4)
            yes = 1;
        return yes;
    }
    return 0;
}

static inline u8 globalMode(void)
{
    u32 active = gPlayerPassageState;
    s32 yes = 0;
    if (active && gPlayerInteractionMode == 4)
        yes = 1;
    return yes;
}

static inline u32 randomDelay(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * 2;
        u32 value;
        value = *(u16 *)(offset + RANDOM_TABLE_ADDR);
        value &= 127;
        value += 120;
        return value;
    }
}
void CollisionController_UpdateIdleAnimation(struct CollisionManager *manager,
                                             struct CollisionSlot *p)
{
    struct SpriteDrawable *o;
    struct DisplaySequenceState *a;
    const u16 *entry;
    u8 done;
    if (p->entered) {
        p->entered = 0;
        Effect_SetOrigin(&p->sprite, -4, -4);
        SpriteDrawable_SelectTileFrame(&p->sprite, manager->animationGraphics, 0);
        Anim_SetSequence(&p->anim, manager->idleResource, 8, 0, 0);
        p->sprite.flags &= ~1;
        p->delay = randomDelay();
    }
    {
        u32 flags = p->sprite.flags;
        if (flags & 1) {
            if (p->timer++ > p->delay) {
                p->timer = 0;
                p->delay = randomDelay();
                Effect_SetOrigin(&p->sprite, -4, -4);
                SpriteDrawable_SelectTileFrame(&p->sprite, manager->animationGraphics, 0);
                Anim_SetSequence(&p->anim, manager->idleResource, 8, 0, 0);
                p->sprite.flags &= ~1;
            }
        } else {
            o = &p->sprite;
            {
                struct SpriteEffectTileResource *graphics = manager->animationGraphics;
                a = &p->anim;
                entry = a->commands + a->index;
                {
                    u32 factor = *entry >> 6;
                    o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->tileBase;
                }
            }
            {
                union DisplaySequenceControl control;
                u32 bit, first;
                control.raw = a->flags;
                bit = 1;
                first = (u8)(control.raw & bit);
                if (first != (((*entry & 0x30) >> 4) & bit))
                    o->flags |= 4;
                else
                    o->flags &= ~4;
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
                            done = 1;
                        } else {
                            a->index = 0;
                            done = 1;
                        }
                    } else
                        done = 0;
                }
                entry = a->commands + a->index;
                a->duration = ((*entry & 15) * a->speed) >> 6;
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
        CollisionController_SetMode(slot, 2);
    }
}

void CollisionController_UpdateHitAnimation(struct CollisionManager *manager,
                                            struct CollisionSlot *p)
{
    struct SpriteDrawable *o;
    struct DisplaySequenceState *a;
    const u16 *entry;
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
    if (p->anim.index == 1 && p->anim.elapsed == 0)
        p->sprite.position.y += 16;
    if (p->anim.index == 2 && p->anim.elapsed == 0)
        p->sprite.position.y -= 16;
    if (p->anim.index == 4 && p->anim.elapsed == 0) {
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
        struct SpriteEffectTileResource *graphics = manager->graphics;
        a = &p->anim;
        entry = a->commands + a->index;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->tileBase;
        }
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            o->flags |= 4;
        else
            o->flags &= ~4;
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
                    done = 1;
                } else {
                    a->index = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        a->duration = ((*entry & 15) * a->speed) >> 6;
    } else
        done = 0;
    p->done = done;
    if (done) {
        p->finished = 1;
        p->sprite.flags |= 1;
    }
}

s32 CollisionController_UpdateAll(struct CollisionManager *manager)
{
    s32 paused = 0;
    s32 i;
    struct CollisionSlot *p;
    u32 mode = gSunClock.phase;
    if ((u8)(mode - 4) <= 1 || mode == 0 || ActorStatus_EnvironmentPaletteActive(0)) {
        paused = 1;
        if (manager->paused == 0)
            for (i = 0; i < 16; i++) {
                struct CollisionSlot *slot = &manager->slots[i];
                if (slot->mode == 0) {
                    CollisionController_SetMode(slot, 0);
                }
            }
    } else if (manager->paused == 1)
        for (i = 0; i < 16; i++) {
            struct CollisionSlot *slot = &manager->slots[i];
            if (slot->mode == 0) {
                CollisionController_SetMode(slot, 0);
            }
        }
    manager->paused = paused;
    for (i = 0; i < 16; i++) {
        p = &manager->slots[i];
        if (p->active) {
            if (p->finished)
                CollisionController_RemoveSlot(manager, p);
            else {
                p->done = 0;
                sub_08249240(manager, p, gUnk_08604A00[paused][p->mode]);
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
    CollisionController_SetManager(NULL);
    return 0;
}

s32 CollisionController_InitManager(struct CollisionManager *manager, void *owner)
{
    u32 mode;
    u8 available;
    CollisionController_SetManager(manager);
    mode = gSunClock.phase;
    if ((((u8)(mode - 4)) <= 1) || (mode == 0)) {
        goto pause;
    }
    if (gEngineFlags & 2048) {
        available = stateMode();
    } else {
        struct Player *actor;
        if (gWeatherCollisionTimer > 0) {
            goto pause;
        }
        actor = gPlayers[0];
        if (actor) {
            if (Player_ReadStatusBits(&actor->actionFlags, 16)) {
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
    struct CollisionManager *p = Task_Create(10, sizeof(*p));
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
    struct ContactPosition position;
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
    Memory_Clear(p, sizeof(*p));
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
    Collider_SetPosition(&p->collider, &position, 0);
    p->sprite.position = position;
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
