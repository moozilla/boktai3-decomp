#include "named_actor_commands.h"

/* NamedObject_ResetManager: Clears the singleton named object manager pointer */

void NamedObject_ResetManager(void)
{
    gNamedActorManager = 0;
}

static inline void orFlags(u16 mask, u16 *flags)
{
    *flags |= mask;
}

void NamedActor_SetFlag20FromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *actor;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor)
            orFlags(0x20, (u16 *)&actor->flags);
    }
}

/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */

static inline u8 HasBit(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

s32 ActorGroup_GetSelectedMemberId(void)
{
    struct NamedObjectManager *group;
    struct NamedObject *member;
    s32 i;
    if (!gNamedActorManager)
        goto fail;
    group = gNamedActorManager;
    if (!group->used)
        goto fail;
    if (TetherActor_TestStatusFlags(1))
        goto scan;
    goto fail;
found:
    return member->name;
scan:
    for (i = 0; i < 12; i++) {
        if (HasBit(group->used, 1 << i)) {
            member = &group->slots[i];
            if (HasBit((u16)member->flags, 0x10))
                goto found;
        }
    }
fail:
    return -1;
}

void NamedActor_SetMoveModeFromScript(void)
{
    s32 mode = Script_GetKeywordValue('m', 1);
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *actor;

    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor && (actor->field_E0 & 2)) {

            actor->state = 2;
            if (mode == 4) {
                NamedObject_DisableFlags(2, actor);
                NamedObject_EnableFlags(mode, actor);
            } else {
                NamedObject_DisableFlags(4, actor);
                if (mode == 2)
                    NamedObject_EnableFlags(mode, actor);
                else
                    NamedObject_DisableFlags(2, actor);
                NamedActor_PlayMovementSound(actor);
            }
        }
    }
}

void NamedActor_SetModeFromScript(void)
{
    s32 r = Script_GetKeywordValue(0x6e, 0);
    if (gNamedActorManager != 0 && r != 0) {
        struct NamedObject *p = NamedObject_FindByName(r);
        p->field_E0 = Script_GetKeywordValue(0x6d, -1);
    }
}

/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */

static inline u8 MemberHasBit(struct NamedObject *m, u32 mask)
{
    if ((u16)m->flags & mask)
        return 1;
    return 0;
}

static inline void ClearFlags(struct NamedObject *m, u32 mask)
{
    m->flags &= ~mask;
}

void ActorGroup_RemoveFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *member;
    struct NamedObjectManager *group;
    if (gNamedActorManager && name) {
        member = NamedObject_FindByName(name);
        group = gNamedActorManager;
        if (MemberHasBit(member, 1)) {
            TerrainModifiers_UnregisterCell(member->registration);
            ClearFlags(member, 1);
        }
        ActorGroup_ReleaseMember(member);
        SceneBody_Unregister(&member->body);
        member->name = 0;
        member->state = 6;
        Sprite_Remove(member);
        {
            s32 index = member->index;
            group->used &= ~(1 << index);
        }
        member->index = -1;
    }
}

void Route_Start(void *route, u32 id, u32 point, u32 mode);

static inline u8 optionalByte(void)
{
    u8 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = 0;
    return value;
}

/* Script: actor -n (looked up by NamedObject_FindByName) starts following route -r id mode
   (Route_Start on its route block at +0xF4, which +0x128 then points to). */

void NamedActor_SetRouteFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *actor;
    u32 id, point, mode;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        id = 0;
        point = 0;
        mode = 0;
        if (Script_SeekToKeyword('r')) {
            id = optionalByte();
            point = 0;
            mode = optionalByte();
        }
        Route_Start(&actor->route, id, point, mode);
        actor->body.currentRoute = &actor->route;
    }
}

void NamedObject_SetPaletteFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *actor;
    if (gNamedActorManager && name && (actor = NamedObject_FindByName(name))) {
        s32 c = Script_GetKeywordValue('c', 0);
        AnimData_SelectPaletteBank(actor->anim, c + 0x23F);
    }
}

void NamedActor_SetSpeedFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *actor;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor) {
            s32 speed = Script_GetKeywordValue('s', 6);
            actor->speed = speed;
        }
    }
}

static inline void clearBits(s16 *p, s32 mask)
{
    *p &= mask;
}

/* Script: undoes a named object's follow (-f in NamedObject_CreateFromScript):
   clears the manager's +0x1370, resets the player's +0x40, Player_ClearAttachmentFlags and
   clears the follow flags (tentative). */

void NamedObject_ReleaseFromScript(void)
{
    struct NamedObjectManager *manager = gNamedActorManager;
    struct NamedObject *actor = NamedObject_GetFollowTarget();
    manager->followName = 0;
    {
        struct Player *player = gPlayers[0];
        player->followTarget = &player->movementContactFlags;
        Player_ClearAttachmentFlags(player, 1);
    }
    clearBits(&actor->flags, ~8);
    gPlayers[0]->shadow[1] &= 0xFE;
}

/* Script: for actor -n, a positive -h goes to +0xD8 and a positive -l to
   +0xD4 (tentative: high and low limits). */

void NamedActor_SetRangeFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct NamedObject *actor;
    if (gNamedActorManager && name) {
        actor = NamedObject_FindByName(name);
        if (actor) {
            s32 none = -1;
            s32 h = Script_GetKeywordValue('h', none);
            s32 l = Script_GetKeywordValue('l', none);
            if (h > 0)
                actor->upperHeight = h;
            if (l > 0)
                actor->lowerHeight = l;
        }
    }
}

void NamedActor_ClearFlag1000FromScript(void)
{
    s32 r = Script_GetKeywordValue(0x6e, 0);
    if (gNamedActorManager != 0 && r != 0) {
        struct NamedObject *p = NamedObject_FindByName(r);
        if (p != 0) {
            u16 *q = (u16 *)&p->flags;
            u32 m = 0xFFFFEFFF;
            *q = m & *q;
        }
    }
}

struct ContactPosition *NamedObject_GetPositionByName(u32 key)
{
    struct NamedObject *actor;

    if (gNamedActorManager && key) {
        actor = NamedObject_FindByName(key);
        if (actor) {
            return &actor->body.position;
        }
    }
    return 0;
}

struct ContactPosition *NamedObject_GetPosition(struct NamedObject *object)
{
    if (gNamedActorManager == 0)
        return 0;
    return &object->body.position;
}

u32 Player_TestAttachmentFlags(void *, u32);

static inline u32 Attributes(s32 cell)
{
    return ((struct ShadowTerrainCell *)SceneActorTerrain_GetCell(cell))->flags;
}

static inline u8 CanExit(s32 cell, s32 height)
{
    if ((*SceneActorTerrain_GetCell(cell) & 15) != height || (Attributes(cell) & 0xC3))
        return 0;
    return 1;
}

static inline u8 PlayExit(s32 cell, s32 height)
{
    if (CanExit(cell, height)) {
        Sound_PlaySong(0x151);
        return 1;
    }
    return 0;
}

void NamedObject_PlayAdjacentExitSound(struct NamedObject *object)
{
    struct NamedObjectManager *manager = gNamedActorManager;
    s32 x, z, cell, height;
    if (!manager)
        return;
    if (!(object->field_E0 & 0x10)) {
        if (!Player_TestAttachmentFlags(gPlayers[0], 1))
            return;
        if (manager->followName != object->name)
            return;
        if (object->field_E0 & 4)
            return;
    }
    {
        u16 *position = (u16 *)&object->body.position;
        x = (s16)position[0] >> 8;
        z = (s16)position[2] >> 8;
    }
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    height = object->body.position.y >> 8;
    if (PlayExit(cell + 1, height))
        return;
    if (PlayExit(cell - 1, height))
        return;
    if (PlayExit(cell + SceneActorTerrain_Width(), height))
        return;
    if (CanExit(cell - SceneActorTerrain_Width(), height))
        Sound_PlaySong(0x151);
}
