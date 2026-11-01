#include "global.h"
#include "engine.h"

#include "named_object.h"
#include "player_cue_sequence.h"
#include "scene_actor_terrain.h"
#include "scene_body_api.h"
#include "player.h"
#include "ram.h"
s32 Path_GetPointPositionById(u16 *, u32, u32);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
void NamedObject_ClearAttachmentFlag(u32, s32);
void Player_AddAttachmentFlags(void *, s32, u32);
u32 TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
void Player_SetPositionAndSave(void *, void *);
void TetherActor_SetPosition(void *);
void TetherActor_AddStatusFlags(s32, u32);
void NamedObject_SnapTetherToPlatform(struct NamedObjectManager *, struct NamedObject *);
static inline void SelectObjectPalette(struct NamedObject *object, s32 index)
{
    AnimData_SelectPaletteBank(object->anim, index);
}
static inline void SetObjectFlags(u32 mask, s16 *flags)
{
    u16 bits = mask;
    bits |= *flags;
    *flags = bits;
}
static inline void ClearObjectFlags(u32 mask, s16 *flags)
{
    *flags = ~mask & (u16)*flags;
}
static inline s32 optionalValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
static inline void readOptional(s32 *dst)
{
    s32 height = optionalValue();
    *dst = height;
}
/* Registry routine A4FC: creates a named scene object (-n) of kind -k (0-3)
   in the gNamedActorManager object pool, with heights -Y, sprite rows -T and
   optional terrain registration (-f). */
void NamedObject_CreateFromScript(void)
{
    struct NamedObjectManager *manager = gNamedActorManager;
    struct NamedObject *e;
    union {
        struct ContactSwarmSprite *sprite;
        struct SceneActorRouteCursor *route;
    } view;
    u32 sequence;
    s32 follow;
    s32 cell;
    struct SceneActorRouteCursor *route;
    struct SceneActorBody *body;
    if (!manager)
        return;
    sequence = 0;
    e = NamedObject_AllocateSlot(manager);
    if (!e)
        return;
    e->flags = sequence;
    e->timer = sequence;
    view.sprite = &e->sprite;
    if (Script_SeekToKeyword('r')) {
        e->routeId = optionalValue();
        e->routeParameter = 0;
        e->routePoint = optionalValue();
        Path_GetPointPositionById((u16 *)&view.sprite->position, e->routeId, e->routePoint);
    }
    e->kind = Script_GetKeywordValue('k', 0);
    if (Script_GetKeywordValue('O', 0))
        SetObjectFlags(0x2000, &e->flags);
    if (Script_SeekToKeyword('Y')) {
        s32 height;
        readOptional(&e->lowerHeight);
        readOptional(&e->upperHeight);
        height = optionalValue();
        if (e->kind == 2) {
            if (height == e->lowerHeight) {
                view.sprite->position.y = height;
                ClearObjectFlags(2, &e->flags);
            } else if (height == e->upperHeight) {
                view.sprite->position.y = height;
                SetObjectFlags(2, &e->flags);
            }
        } else
            view.sprite->position.y = e->lowerHeight;
        e->body.position.y = view.sprite->position.y;
    }
    e->name = Script_GetKeywordValue('n', 0);
    e->enterScript = Script_GetKeywordValue('p', 0);
    e->script = Script_GetKeywordValue('R', 0);
    e->exitScript = Script_GetKeywordValue('S', 0);
    e->row = Script_GetKeywordValue('T', 0);
    AnimData_Load(e->anim, 0xB019);
    switch (e->kind) {
        case 0: {
            s32 m;
            view.sprite->position.y = e->lowerHeight;
            e->state = 1;
            e->handlers[0] = NamedObject_TryCapturePlayer;
            e->handlers[1] = NamedObject_ClampTetherAtBarrier;
            e->handlers[2] = NamedObject_UpdateTerrainRegistration;
            sequence = 1;
            AnimData_SelectPaletteBank(e->anim, 0x240);
            m = Script_GetKeywordValue('m', 0);
            if (m)
                e->field_E0 = m;
            e->row = 0;
            break;
        }
        case 3: {
            s32 m;
            view.sprite->position.y = e->lowerHeight;
            e->state = 1;
            e->handlers[0] = NamedObject_TryCapturePlayer;
            e->handlers[1] = NamedObject_ClampTetherAtBarrier;
            e->handlers[2] = NamedObject_UpdateTerrainRegistration;
            sequence = 1;
            AnimData_SelectPaletteBank(e->anim, 0x246);
            m = Script_GetKeywordValue('m', 0);
            if (m)
                e->field_E0 = m;
            e->row = 0;
            break;
        }
        case 1: {
            s32 m;
            view.sprite->position.y = e->lowerHeight;
            e->state = 1;
            e->handlers[0] = NamedObject_UpdateEntryTrigger;
            e->handlers[1] = NamedObject_ClampTetherToPlatform;
            e->handlers[2] = NamedObject_UpdateTerrainRegistration;
            sequence = 2;
            switch (e->row) {
                case 0:
                    AnimData_SelectPaletteBank(e->anim, 0x241);
                    break;
                case 1:
                    AnimData_SelectPaletteBank(e->anim, 0x244);
                    break;
                case 2:
                    AnimData_SelectPaletteBank(e->anim, 0x245);
                    break;
            }
            m = Script_GetKeywordValue('m', 0);
            if (m)
                e->field_E0 = m;
            if (e->row > 2)
                e->row = 0;
            break;
        }
        case 2:
            e->state = 4;
            e->handlers[0] = NamedObject_ConstrainPlayer;
            e->handlers[1] = NamedObject_ConstrainTether;
            e->handlers[2] = NamedObject_UpdateTerrainRegistration;
            sequence = 0;
            switch (e->row) {
                case 0:
                    SelectObjectPalette(e, 0x23F);
                    break;
                case 1:
                    SelectObjectPalette(e, 0x241);
                    break;
                case 2:
                    SelectObjectPalette(e, 0x246);
                    break;
                case 3:
                    SelectObjectPalette(e, 0x245);
                    break;
                case 4:
                    SelectObjectPalette(e, 0x247);
                    break;
            }
            e->field_E0 = Script_GetKeywordValue('m', 1);
            e->limit = Script_GetKeywordValue('w', 0x78);
            if (Script_GetKeywordValue('M', 0))
                e->timer = e->limit;
            break;
    }
    Sprite_Init(view.sprite, e->anim, 0);
    Anim_SetSequence(&e->sequence, manager->sequences, (u16)sequence, (u8)e->row, 0);
    PlayerCue_StepSequence(view.sprite, &e->sequence);
    view.sprite->flags &= ~1;
    view.sprite->priority = 2;
    body = &e->body;
    SceneBody_Init(body, e->index + 0xD0E9, &view.sprite->position, 0, 7, e);
    SceneBody_SetSprite(body, view.sprite);
    route = &e->route;
    SceneBody_StartRoute(body, &e->route, e->routeId, e->routeParameter, e->routePoint);
    body->movementProbe = 0;
    view.sprite->position.x -= 0x100;
    {
        s32 enabled = Script_GetKeywordValue('d', 0);
        view.route = route;
        if (enabled)
            SetObjectFlags(0x80, &e->flags);
    }
    if (Script_GetKeywordValue('F', 0))
        SetObjectFlags(0x1000, &e->flags);
    e->stage = 0;
    e->offsetC2 = 0;
    do {
        follow = Script_GetKeywordValue('f', 0);
    } while (0);
    if (follow) {
        switch (e->kind) {
            case 0:
            case 1:
            case 3:
                e->state = 1;
                break;
            case 2:
                e->state = 4;
                break;
        }
        NamedObject_ClearAttachmentFlag(manager->followName, 0);
        manager->followName = e->name;
        Player_AddAttachmentFlags(gPlayers[0], 1, e->name);
        SetObjectFlags(8, &e->flags);
        gPlayers[0]->shadow[1] |= 1;
        cell = SceneActorTerrain_GetCellIndex(&e->body.position);
        TerrainModifiers_RegisterCell(e->registration, cell, 0, e->body.position.y >> 8, 0xFF, 4);
        NamedObject_EnableFlags(1, e);
        Player_SetPositionAndSave(gPlayers[0], &e->body.position);
        if (follow == 2) {
            TetherActor_SetPosition(&e->body.position);
            NamedObject_ClearAttachmentFlag(manager->tetherName, 1);
            manager->tetherName = e->name;
            TetherActor_AddStatusFlags(3, e->name);
            NamedObject_EnableFlags(0x10, e);
        }
    }
    NamedObject_SnapTetherToPlatform(manager, e);
    e->speed = Script_GetKeywordValue('s', 6);
    switch (e->kind) {
        case 0:
        case 1:
        case 3:
            if ((u8)NamedObject_IsWithinRoutePointBounds(view.route, &e->body.position,
                                                         (s16)e->speed, (s16)e->speed))
                e->state = 3;
            break;
    }
    e->unknownBB = 0;
}
