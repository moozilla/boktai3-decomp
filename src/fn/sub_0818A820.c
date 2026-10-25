#include "global.h"

// confidence=low: per-frame armor actor update; drains queued messages, then moves, collides and runs callbacks.

struct Armor08A820Vec {
    s16 x, y, z, pad;
};

struct Armor08A820Slot {
    void *fn;
    u8 pad[0x3C];
};

struct Armor08A820Body {
    u16 type;
};

struct Armor08A820Record {
    u32 unk0;
    struct Armor08A820Body *body;
};

struct Armor08A820Entry {
    struct Armor08A820Record *record;
    u32 unk4;
};

struct Armor08A820Actor {
    u8 pad000[0x44];
    u8 unk44;
    u8 pad045[0x3];
    struct Armor08A820Vec vel;
    struct Armor08A820Vec pos;
    void *movement;
    u8 pad05C[0x28];
    u8 unk84;
    u8 pad085[0x51];
    u16 unkD6;
    u8 pad0D8[0xAC];
    u32 unk184;
    u8 pad188[0x8];
    u8 colliderB[0x54];
    u8 colliderA[0x54];
    u8 colliderC[0x54];
    u8 colliderD[0x54];
    u8 pad2E0[0x70];
    void *cb350;
    u8 pad354[0x3C];
    void *cb390;
    u8 pad394[0x3C];
    void *cb3D0;
    u8 pad3D4[0x3C];
    struct Armor08A820Slot slots[9];
    void *cb650;
    u8 pad654[0x4BC];
    s16 velX;
    u8 padB12[0x2];
    s16 velZ;
    u8 padB16[0x2];
    u32 messageBox;
    u8 padB1C[0x10];
    s16 timer2C;
    u8 padB2E[0x4];
    u16 flags;
    u8 padB34[0x5];
    u8 unkB39;
    u8 padB3A[0x2];
    u8 behavior;
    u8 phaseB3D;
    u8 padB3E[0x8];
    u16 timer46;
    void *attachment;
    u8 padB4C[0xC];
    u8 effectPool[4];
};

#define ARMOR08A820_U16(p, off) (*(u16 *)((u8 *)(p) + (off)))

u32 MessageQueue_GetMessagesById(u32 id, void *out);
void ActorController_SetBehavior(void *actor, s32 behavior);
void Armor_ApplySceneCommands(void *actor);
void Armor_DispatchTypeUpdate(void *actor);
void Armor_DispatchBehaviorUpdate(void *actor);
void ActorController_UpdateAnimation(void *actor);
void SceneBody_SetMovementProbe(void *body, void *target, s32 a, s32 b);
void SceneBody_ClearMovementProbe(void *body);
s32 sub_0821C254(void *movement, void *vel, void *tmp, s32 mode);
void Obj_StepPosition(void *body);
u8 *TerrainModifiers_FindCellOverride(s32 cell, s32 flag);
s32 Collider_Refresh(void *collider);
void Collider_SetPosition(void *collider, void *position, u32 value);
s32 Callback_InvokeActor(void *arg, void *function);
void Armor_StartAccentPaletteFade(void *actor);
void SceneEffectPool_Update(void *pool, void *vel);
void ActorAttachment_ReleaseResource(void *attachment, s32 mode, u32 id);

// confidence=low: per-frame armor actor update; drains queued messages, then moves, collides and runs callbacks.
static inline s16 Decay08A820(s16 v)
{
    if (v == -1)
        return 0;
    return v >> 1;
}

#ifdef NONMATCHING
// fuzzy: 54.8%
// scope shape 230 (differences once low registers are set aside)
s32 sub_0818A820(void *arg0)
{
    struct Armor08A820Actor *actor = arg0;
    struct Armor08A820Entry *entry;
    struct Armor08A820Vec tmp;
    u32 buf[1];
    s32 count;
    s32 i;

    count = MessageQueue_GetMessagesById(actor->messageBox, buf);
    entry = (struct Armor08A820Entry *)buf;
    while (count > 0) {
        switch (entry->record->body->type) {
            case 0:
                if (actor->flags & 0x8000) {
                    actor->flags &= 0x73FF;
                    if (actor->behavior == 0x2C) {
                        ActorController_SetBehavior(actor, 0x2D);
                    } else {
                        ActorController_SetBehavior(actor, 1);
                        actor->phaseB3D = 0xFF;
                    }
                    ARMOR08A820_U16(actor, 0x1EA) &= 0xFFFB;
                    ARMOR08A820_U16(actor, 0x23E) &= 0xFFFB;
                    actor->unk184 &= ~2;
                }
                break;
            case 1:
                if (!(actor->flags & 0x8000)) {
                    actor->flags |= 0x8000;
                    ActorController_SetBehavior(actor, 0);
                    ARMOR08A820_U16(actor, 0x1EA) |= 4;
                    ARMOR08A820_U16(actor, 0x23E) |= 4;
                    actor->unk184 |= 1;
                }
                break;
            case 2:
                if (actor->flags & 0x8000) {
                    actor->flags &= 0xF3FF;
                    if (actor->behavior == 0x2C)
                        ActorController_SetBehavior(actor, 0x2D);
                }
                break;
            case 3:
                if (!(actor->flags & 0x1000)) {
                    actor->flags |= 0x1000;
                    ARMOR08A820_U16(actor, 0x1EA) |= 4;
                    ARMOR08A820_U16(actor, 0x23E) |= 4;
                    actor->unk184 |= 1;
                }
                break;
            case 4:
                if (actor->flags & 0x1000)
                    actor->flags &= 0xEFFF;
                break;
        }
        entry++;
        count--;
    }

    if (actor->flags & 0x1000)
        return 0;

    if (actor->timer2C > 0)
        actor->timer2C--;
    if (actor->flags & 0x8000)
        Armor_ApplySceneCommands(actor);
    else
        Armor_DispatchTypeUpdate(actor);
    Armor_DispatchBehaviorUpdate(actor);
    ActorController_UpdateAnimation(actor);

    if (actor->flags & 0x40) {
        actor->pos.x += actor->velX;
        actor->pos.z += actor->velZ;
        actor->velX = Decay08A820(actor->velX);
        actor->velZ = Decay08A820(actor->velZ);
        if (actor->velX == 0 && actor->velZ == 0)
            actor->flags &= 0xFFBF;
    }

    if (actor->flags & 0x60) {
        if (actor->movement == NULL)
            SceneBody_SetMovementProbe((u8 *)actor + 0x3C, &actor->unk84, 0x40, 0x40);
    } else if (actor->movement != NULL) {
        SceneBody_ClearMovementProbe((u8 *)actor + 0x3C);
    }

    if (actor->movement != NULL) {
        tmp.x = actor->pos.x + actor->vel.x;
        tmp.y = actor->pos.y + actor->vel.y;
        tmp.z = actor->pos.z + actor->vel.z;
        if (sub_0821C254(actor->movement, &actor->vel, &tmp, actor->unk44)) {
            actor->pos.x = tmp.x - actor->vel.x;
            actor->pos.y = tmp.y - actor->vel.y;
            actor->pos.z = tmp.z - actor->vel.z;
        }
    }
    Obj_StepPosition((u8 *)actor + 0x3C);

    if (actor->movement != NULL) {
        if (actor->unk84 & 1)
            actor->flags |= 0x10;
        else
            actor->flags &= 0xFFEF;
    } else {
        actor->flags &= 0xFFEF;
    }

    if (actor->movement == NULL) {
        s32 tileX = (s8)(actor->vel.x >> 8);
        s32 tileZ = (s8)(actor->vel.z >> 8);
        s32 cell;
        u8 *entryCell;
        s32 value;

        if (tileX < 0 || tileZ < 0 || tileX >= *(s32 *)0x030052F8 || tileZ >= *(s32 *)0x030052FC)
            cell = 0;
        else
            cell = *(u16 *)(*(u8 **)0x030052F4 + 0x24 + tileZ * 2) + tileX;

        entryCell = TerrainModifiers_FindCellOverride(cell, 1);
        if (entryCell == NULL)
            entryCell = *(u8 **)(*(u8 **)0x030052F4 + 4) + 0xC + cell * 4;
        else
            entryCell += 4;

        value = (*entryCell & 0xF) << 8;
        if ((*entryCell >> 4) == 1)
            value -= *((u8 *)&actor->vel + 4);
        else if ((*entryCell >> 4) == 2)
            value -= *((u8 *)&actor->vel);
        actor->vel.y = value;
        actor->unkD6 = value;
    }

    Collider_Refresh(actor->colliderA);
    Collider_Refresh(actor->colliderB);
    Collider_SetPosition(actor->colliderA, &actor->vel, 0);
    Collider_SetPosition(actor->colliderC, &actor->vel, 0);
    Collider_SetPosition(actor->colliderD, &actor->vel, 0);
    Collider_SetPosition(actor->colliderB, &actor->vel, 0);
    ARMOR08A820_U16(actor, 0x226) = actor->unkB39;
    ARMOR08A820_U16(actor, 0x27A) = actor->unkB39;

    if (actor->cb350 != NULL)
        Callback_InvokeActor(&actor->cb350, actor->cb350);
    if (actor->cb390 != NULL)
        Callback_InvokeActor(&actor->cb390, actor->cb390);
    if (actor->cb3D0 != NULL)
        Callback_InvokeActor(&actor->cb3D0, actor->cb3D0);
    for (i = 0; i <= 8; i++) {
        struct Armor08A820Slot *slot = &actor->slots[i];
        if (slot->fn != NULL)
            Callback_InvokeActor(slot, slot->fn);
    }
    Armor_StartAccentPaletteFade(actor);
    if (actor->cb650 != NULL)
        Callback_InvokeActor(&actor->cb650, actor->cb650);
    SceneEffectPool_Update(actor->effectPool, &actor->vel);

    if (actor->attachment != NULL) {
        actor->timer46--;
        if ((s16)actor->timer46 <= 0) {
            ActorAttachment_ReleaseResource(actor->attachment, 1, actor->messageBox);
            ActorAttachment_ReleaseResource(actor->attachment, 0, actor->messageBox);
            actor->attachment = NULL;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0818A820);
#endif
