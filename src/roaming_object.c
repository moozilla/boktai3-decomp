#include "roaming_object.h"
#include "roaming_object_playback.h"

extern const struct RoamingStateTable gUnk_0824DF8C;

void RoamingObject_SetState(struct RoamingObject *owner, u16 state)
{
    struct RoamingStateTable table = gUnk_0824DF8C;
    owner->state = state;
    owner->flashCount = 0;
    owner->update = table.entries[owner->state];
}

s32 RoamingObject_OnDamage(struct SceneActorBody *source, u32 event, s32 amount)
{
    struct RoamingObject *owner = source->owner;
    if (event == 0) {
        if (owner->state != 2) {
            if (amount > 0) {
                struct ContactPackedVector offset;
                offset.x = event;
                offset.y = event;
                offset.z = 0;
                DamageNumberDisplay_Show(&owner->collider, amount, 0, &owner->body.position,
                                         &offset, 1);
            }
            RoamingObject_SetState(owner, 1);
        }
    } else if (event == 1) {
        owner->impulse = amount;
        {
            struct RoamingObjectListener *listener = &owner->listener;
            listener->flags |= event;
        }
        RoamingObject_SetState(owner, 2);
    }
    return 1;
}

void RoamingObject_SteerWander(struct RoamingObject *owner)
{
    u32 index_2;
    if (owner->body.direction != owner->targetAngle) {
        if ((((owner->targetAngle - owner->body.direction) + 256) & 255) <= 127) {
            owner->body.direction += 4;
        } else {
            owner->body.direction += 252;
        }
    }
    if (owner->turnTimer == 0) {
        const u16 *table = gRandomTable;
        u32 *counter = &gRandomIndex;
        u32 index;
        u32 mask;
        u32 sample;
        index = *counter + 1;
        mask = 1023;
        index &= mask;
        *(&sample) = table[index];
        sample &= 3;
        sample <<= 6;
        owner->targetAngle = sample;
        index_2 = (index + 1) & mask;
        *counter = index_2;
        sample = table[index_2];
        sample &= 63;
        sample += 120;
        owner->turnTimer = sample;
    } else {
        owner->turnTimer--;
    }
    Collider_SetPosition(&owner->collider, &owner->body.position, 0);
    Collider_Refresh(&owner->collider);
    owner->animation = 7;
}

void RoamingObject_FlashPalette(struct RoamingObject *owner)
{
    if (owner->flashCount <= 5)
        AnimData_SelectPaletteBank(&owner->graphics, 0x192);
    else
        AnimData_SelectPaletteBank(&owner->graphics, 0x193);
    owner->animation = 2;
    owner->flashCount++;
}

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

void RoamingObject_ApplyImpulse(struct RoamingObject *owner)
{
    owner->animation = 2;
    if (owner->impulse) {
        s32 x = owner->impulse;
        const s16 *table = gUnk_086149C4;
        x = Scale(table[(owner->body.direction + 64) & 255] * x);

        owner->body.motion.x += x;
        {
            s32 y = owner->impulse;
            owner->body.motion.z += Scale(y * gUnk_086149C4[owner->body.direction]);
        }
        owner->impulse = (owner->impulse * 3) >> 2;
    } else {
        owner->body.direction += 128;
        owner->targetAngle = owner->body.direction;
        {
            struct RoamingObjectListener *listener = &owner->listener;
            s32 mask = -2;
            listener->flags &= mask;
        }
        RoamingObject_SetState(owner, 0);
    }
}

void RoamingObject_UpdateAnimation(struct RoamingObject *owner)
{
    s32 angle = (owner->body.direction + 32) & 255;
    s32 mask;
    s32 row;
    u32 mirror;
    struct DisplaySequenceState *sequence;
    *(&angle) >>= 6;
    angle++;
    mask = 3;
    angle &= mask;
    if (angle > 1) {
        row = mask - angle;
        mirror = 1;
    } else {
        row = angle;
        mirror = 0;
    }
    sequence = &owner->sequence;
    Anim_SetSequence(sequence, owner->sequenceResource, owner->animation, (u8)row, mirror);
    RoamingObject_StepPlayback(owner, sequence);
}

void RoamingObject_UpdateAnimation(struct RoamingObject *);

void Obj_StepPosition(void *);

u32 RoamingObject_Tick(struct RoamingObject *owner)
{
    owner->update(owner);
    RoamingObject_UpdateAnimation(owner);
    Obj_StepPosition(&owner->body);
    return 0;
}

s32 RoamingObject_Destroy(struct RoamingObject *owner)
{
    Sprite_Remove(&owner->sprite);
    SceneBody_Unregister(&owner->body);
    SceneLink_RemoveNode(&owner->listener);
    return 0;
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);

void RoamingObject_InitializePosition(struct RoamingObject *owner)
{
    struct ContactPackedVector position;
    u16 *point;
    s32 x, z;
    u32 cell;
    struct ShadowTerrainCell *definition;
    u32 type;
    s32 height;
    if (Script_SeekToKeyword(0x70)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    point = (u16 *)&position;
    x = (s16)point[0] >> 8;
    z = (s16)point[2] >> 8;
    if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    definition = (void *)TerrainModifiers_FindCellOverride(cell, 1);
    if (definition)
        definition = &((struct ShadowTerrainOverride *)definition)->cell;
    else {
        struct EngineMap *map = gTerrainMap;
        definition = &((struct ShadowTerrainGrid *)map->cells)->cells[cell];
    }
    type = definition->height >> 4;
    height = (definition->height & 15) << 8;
    switch (type) {
        case 1:
            height -= (u8)point[2];
            break;
        case 2:
            height -= (u8)point[0];
            break;
    }
    ((u16 *)&position)[1] = height;
    SceneBody_Init(&owner->body, owner->id, &position, 0, 7, owner);
}

void RoamingObject_InitHomeBounds(struct RoamingObject *owner)
{
    struct RoamingObjectCellProbe *home = &owner->home;
    CellObject_Init(home, &owner->body.position);
    SceneBody_SetMovementProbe(&owner->body, home, 30, 30);
}

void RoamingObject_InitGraphics(struct RoamingObject *owner)
{
    struct SceneAnimation *graphics = &owner->graphics;
    struct ContactSwarmSprite *sprite;
    AnimData_Load(graphics, 0x9247);
    sprite = &owner->sprite;
    Sprite_Init(sprite, graphics, 0);
    SceneBody_SetSprite(&owner->body, sprite);
    AnimData_SelectPaletteBank(graphics, 0x193);
    owner->sprite.position = owner->body.position;
    owner->sequenceResource = ScriptProc_Start(0x922E, 0x1DF8);
    owner->animation = 7;
    RoamingObject_UpdateAnimation(owner);
}

void RoamingObject_InitCollider(struct RoamingObject *owner)
{
    struct RoamingObjectExtent size, offset;
    struct ContactCollider *collider = &owner->collider;
    u32 category;
    size.xy.first = 64;
    size.xy.second = 100;
    size.z.first = 64;
    *(u32 *)&offset.xy = 0x640000;
    *(u32 *)&offset.z &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x2001, 0, category = 16, &size, &offset);
    Collider_SetAttackProperties(collider, 10, 30, 0, 0, category);
    Collider_SetCallback(collider, 0, owner);
}

s32 RoamingObject_Initialize(struct RoamingObject *owner, u32 id, u32 argument)
{
    owner->id = id;
    RoamingObject_InitializePosition(owner);
    RoamingObject_InitHomeBounds(owner);
    RoamingObject_InitGraphics(owner);
    RoamingObject_InitCollider(owner);
    TargetRegistry_RegisterActor(&owner->listener, &owner->body, RoamingObject_OnDamage);
    RoamingObject_SetState(owner, 0);
    return 0;
}

struct RoamingObject *RoamingObject_Create(u32 id, u32 argument)
{
    struct RoamingObject *owner = Task_Create(8, sizeof(*owner));
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))RoamingObject_Tick,
                          (void (*)(void *))RoamingObject_Destroy);
        if (RoamingObject_Initialize(owner, id, argument) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}

extern const struct RoamingStateTable gUnk_0824DF98;

void RoamingObjectB_SetState(struct RoamingObject *owner, u16 state)
{
    struct RoamingStateTable table = gUnk_0824DF98;
    owner->state = state;
    owner->flashCount = 0;
    owner->update = table.entries[owner->state];
}

s32 RoamingObjectB_OnDamage(struct ContactCollider *source, u32 event, s32 amount)
{
    struct RoamingObject *owner = source->owner;
    if (event == 0) {
        if (owner->state != 2) {
            if (amount > 0) {
                struct ContactPackedVector offset;
                offset.x = event;
                offset.y = event;
                offset.z = 0;
                DamageNumberDisplay_Show(&owner->collider, amount, 0, &owner->body.position,
                                         &offset, 1);
            }
            RoamingObjectB_SetState(owner, 1);
        }
    } else if (event == 1) {
        owner->impulse = amount;
        {
            struct RoamingObjectListener *listener = &owner->listener;
            listener->flags |= event;
        }
        RoamingObjectB_SetState(owner, 2);
    }
    return 1;
}

void RoamingObjectB_SteerWander(struct RoamingObject *owner)
{
    u32 index_2;
    if (owner->body.direction != owner->targetAngle) {
        if ((((owner->targetAngle - owner->body.direction) + 256) & 255) <= 127) {
            owner->body.direction += 4;
        } else {
            owner->body.direction += 252;
        }
    }
    if (owner->turnTimer == 0) {
        const u16 *table = gRandomTable;
        u32 *counter = &gRandomIndex;
        u32 index;
        u32 mask;
        u32 sample;
        index = *counter + 1;
        mask = 1023;
        index &= mask;
        *(&sample) = table[index];
        sample &= 3;
        sample <<= 6;
        owner->targetAngle = sample;
        index_2 = (index + 1) & mask;
        *counter = index_2;
        sample = table[index_2];
        sample &= 63;
        sample += 120;
        owner->turnTimer = sample;
    } else {
        owner->turnTimer--;
    }
    Collider_SetPosition(&owner->collider, &owner->body.position, 0);
    Collider_Refresh(&owner->collider);
    owner->animation = 7;
}

void RoamingObjectB_FlashPalette(struct RoamingObject *owner)
{
    if (owner->flashCount <= 5)
        AnimData_SelectPaletteBank(&owner->graphics, 0x192);
    else
        AnimData_SelectPaletteBank(&owner->graphics, 0x193);
    owner->animation = 2;
    owner->flashCount++;
}

void RoamingObjectB_ApplyImpulse(struct RoamingObject *owner)
{
    owner->animation = 2;
    if (owner->impulse) {
        s32 x = owner->impulse;
        const s16 *table = gUnk_086149C4;
        x = Scale(table[(owner->body.direction + 64) & 255] * x);

        owner->body.motion.x += x;
        {
            s32 y = owner->impulse;
            owner->body.motion.z += Scale(y * gUnk_086149C4[owner->body.direction]);
        }
        owner->impulse = (owner->impulse * 3) >> 2;
    } else {
        owner->body.direction += 128;
        owner->targetAngle = owner->body.direction;
        {
            struct RoamingObjectListener *listener = &owner->listener;
            s32 mask = -2;
            listener->flags &= mask;
        }
        RoamingObjectB_SetState(owner, 0);
    }
}

void RoamingObjectB_UpdateAnimation(struct RoamingObject *owner)
{
    s32 angle = (owner->body.direction + 32) & 255;
    s32 mask;
    s32 row;
    u32 mirror;
    struct DisplaySequenceState *sequence;
    *(&angle) >>= 6;
    angle++;
    mask = 3;
    angle &= mask;
    if (angle > 1) {
        row = mask - angle;
        mirror = 1;
    } else {
        row = angle;
        mirror = 0;
    }
    sequence = &owner->sequence;
    Anim_SetSequence(sequence, owner->sequenceResource, owner->animation, (u8)row, mirror);
    RoamingObject_StepPlayback(owner, sequence);
}

void RoamingObjectB_UpdateAnimation(struct RoamingObject *);

u32 RoamingObjectB_Tick(struct RoamingObject *owner)
{
    owner->update(owner);
    RoamingObjectB_UpdateAnimation(owner);
    Obj_StepPosition(&owner->body);
    return 0;
}

s32 RoamingObjectB_Destroy(struct RoamingObject *owner)
{
    Sprite_Remove(&owner->sprite);
    SceneBody_Unregister(&owner->body);
    SceneLink_RemoveNode(&owner->listener);
    return 0;
}

void RoamingObjectB_InitializePosition(struct RoamingObject *owner)
{
    struct ContactPackedVector position;
    u16 *point;
    s32 x, z;
    u32 cell;
    struct ShadowTerrainCell *definition;
    u32 type;
    s32 height;
    if (Script_SeekToKeyword(0x70)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    point = (u16 *)&position;
    x = (s16)point[0] >> 8;
    z = (s16)point[2] >> 8;
    if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    definition = (void *)TerrainModifiers_FindCellOverride(cell, 1);
    if (definition)
        definition = &((struct ShadowTerrainOverride *)definition)->cell;
    else {
        struct EngineMap *map = gTerrainMap;
        definition = &((struct ShadowTerrainGrid *)map->cells)->cells[cell];
    }
    type = definition->height >> 4;
    height = (definition->height & 15) << 8;
    switch (type) {
        case 1:
            height -= (u8)point[2];
            break;
        case 2:
            height -= (u8)point[0];
            break;
    }
    ((u16 *)&position)[1] = height;
    SceneBody_Init(&owner->body, owner->id, &position, 0, 7, owner);
}

void RoamingObjectB_InitHomeBounds(struct RoamingObject *owner)
{
    struct RoamingObjectCellProbe *home = &owner->home;
    CellObject_Init(home, &owner->body.position);
    SceneBody_SetMovementProbe(&owner->body, home, 30, 30);
}

void RoamingObjectB_InitGraphics(struct RoamingObject *owner)
{
    struct SceneAnimation *graphics = &owner->graphics;
    struct ContactSwarmSprite *sprite;
    AnimData_Load(graphics, 0x9247);
    sprite = &owner->sprite;
    Sprite_Init(sprite, graphics, 0);
    SceneBody_SetSprite(&owner->body, sprite);
    AnimData_SelectPaletteBank(graphics, 0x193);
    owner->sprite.position = owner->body.position;
    owner->sequenceResource = ScriptProc_Start(0x922E, 0x1DF8);
    owner->animation = 7;
    RoamingObjectB_UpdateAnimation(owner);
}

void RoamingObjectB_InitCollider(struct RoamingObject *owner)
{
    struct RoamingObjectExtent size, offset;
    struct ContactCollider *collider = &owner->collider;
    u32 category;
    size.xy.first = 64;
    size.xy.second = 100;
    size.z.first = 64;
    *(u32 *)&offset.xy = 0x640000;
    *(u32 *)&offset.z &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x2001, 0, category = 16, &size, &offset);
    Collider_SetAttackProperties(collider, 10, 30, 0, 0, category);
    Collider_SetCallback(collider, 0, owner);
}

s32 RoamingObjectB_Initialize(struct RoamingObject *owner, u32 id, u32 argument)
{
    owner->id = id;
    RoamingObjectB_InitializePosition(owner);
    RoamingObjectB_InitHomeBounds(owner);
    RoamingObjectB_InitGraphics(owner);
    RoamingObjectB_InitCollider(owner);
    TargetRegistry_RegisterSceneActor(&owner->listener, &owner->collider, RoamingObjectB_OnDamage);
    RoamingObjectB_SetState(owner, 0);
    return 0;
}

struct RoamingObject *RoamingObjectB_Create(u32 id, u32 argument)
{
    struct RoamingObject *owner = Task_Create(8, sizeof(*owner));
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))RoamingObjectB_Tick,
                          (void (*)(void *))RoamingObjectB_Destroy);
        if (RoamingObjectB_Initialize(owner, id, argument) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
