#include "global.h"
#include "engine.h"
#include "terrain_cell_probe.h"
#include "display_animation.h"
#include "player_cue_sequence.h"
#include "player.h"
#include "ram.h"
/* Packed so that flag updates store through a field insertion. */
struct NamedObjectFlags {
    u16 value;
} __attribute__((packed, aligned(2)));
struct NamedObjectSprite {
    u32 flags;
    u8 pad4[3];
    u8 field7;
    u8 pad8[8];
    u16 frame;
    u8 pad12[0xA];
    u16 x, y;
    u8 pad20[0xC];
};
struct NamedObjectBody {
    u8 pad0[0xC];
    s16 x, height, z;
    u8 pad12[0xA];
    u32 field1C;
    u8 pad20[0x28];
    u8 field48[0x54];
};
struct NamedObject {
    struct NamedObjectSprite sprite;
    u8 anim[0x1C];
    struct DisplaySequenceState sequence;
    u8 pad58[0x5C];
    s16 index;
    u16 state;
    struct NamedObjectFlags flags;
    u8 fieldBA, fieldBB;
    u16 fieldBC, fieldBE, fieldC0, fieldC2;
    u32 fieldC4, fieldC8, fieldCC;
    u16 row;
    u8 padD2[2];
    u32 heightLow, heightHigh;
    u32 name;
    u32 fieldE0;
    void *handlers[3];
    u8 fieldF0, fieldF1, fieldF2, kind;
    u8 fieldF4[0xC];
    struct NamedObjectBody body;
};
struct NamedObjectManager {
    u8 pad0[0x18];
    void *sequences;
    u8 pad1C[0x1354];
    u32 field1370;
    u32 field1374;
};
static inline void SelectPlatformPalette(struct NamedObject *e)
{
    switch (e->row) {
        case 0:
            AnimData_SelectPaletteBank(e->anim, 0x23F);
            break;
        case 1:
            AnimData_SelectPaletteBank(e->anim, 0x241);
            break;
        case 2:
            AnimData_SelectPaletteBank(e->anim, 0x246);
            break;
        case 3:
            AnimData_SelectPaletteBank(e->anim, 0x245);
            break;
        case 4:
            AnimData_SelectPaletteBank(e->anim, 0x247);
            break;
    }
}
struct NamedObject *NamedObject_AllocateSlot(struct NamedObjectManager *);
void Path_GetPointPositionById(void *, u32, u32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
void NamedObject_ClearAttachmentFlag(u32, s32);
void Player_AddAttachmentFlags(void *, s32, u32);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void Player_SetPositionAndSave(void *, void *);
void TetherActor_SetPosition(void *);
void TetherActor_AddStatusFlags(s32, u32);
void NamedObject_SnapTetherToPlatform(struct NamedObjectManager *, struct NamedObject *);
u8 NamedObject_IsWithinSpriteBounds(void *, void *, s32, s32);
void NamedObject_TryCapturePlayer(void);
void NamedObject_ClampTetherAtBarrier(void);
void NamedObject_UpdateTerrainRegistration(void);
void NamedObject_UpdateEntryTrigger(void);
void NamedObject_ClampTetherToPlatform(void);
void NamedObject_ConstrainPlayer(void);
void NamedObject_ConstrainTether(void);
static inline void SetFlagComponent(struct NamedObjectFlags *field, u16 bits)
{
    field->value = bits | field->value;
}
static inline void ClearFlagComponent(struct NamedObjectFlags *field, u32 bits)
{
    u32 mask = ~bits;
    field->value = mask & field->value;
}
static inline s32 optionalValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
static inline void readOptional(u32 *dst)
{
    *dst = optionalValue();
}
/* Registry routine A4FC: creates a named scene object (-n) of kind -k (0-3)
   in the gNamedActorManager object pool, with heights -Y, sprite rows -T and
   optional terrain registration (-f). */
void NamedObject_CreateFromScript(void)
{
    struct NamedObjectManager *manager = gNamedActorManager;
    struct NamedObject *e;
    struct NamedObjectSprite *sprite;
    u32 sequence;
    s32 follow;
    s32 cell;
    if (!manager)
        return;
    sequence = 0;
    e = NamedObject_AllocateSlot(manager);
    if (!e)
        return;
    e->flags.value = sequence;
    e->fieldBC = sequence;
    sprite = &e->sprite;
    if (Script_SeekToKeyword('r')) {
        e->fieldF0 = optionalValue();
        e->fieldF1 = 0;
        e->fieldF2 = optionalValue();
        Path_GetPointPositionById(&sprite->x, e->fieldF0, e->fieldF2);
    }
    e->kind = Script_GetKeywordValue('k', 0);
    if (Script_GetKeywordValue('O', 0))
        SetFlagComponent(&e->flags, 0x2000);
    if (Script_SeekToKeyword('Y')) {
        s32 height;
        readOptional(&e->heightLow);
        readOptional(&e->heightHigh);
        height = optionalValue();
        if (e->kind == 2) {
            if (height == e->heightLow) {
                sprite->y = height;
                ClearFlagComponent(&e->flags, 2);
            } else if (height == e->heightHigh) {
                sprite->y = height;
                SetFlagComponent(&e->flags, 2);
            }
        } else
            sprite->y = e->heightLow;
        e->body.height = sprite->y;
    }
    e->name = Script_GetKeywordValue('n', 0);
    e->fieldC4 = Script_GetKeywordValue('p', 0);
    e->fieldC8 = Script_GetKeywordValue('R', 0);
    e->fieldCC = Script_GetKeywordValue('S', 0);
    e->row = Script_GetKeywordValue('T', 0);
    AnimData_Load(e->anim, 0xB019);
    switch (e->kind) {
        case 0: {
            s32 m;
            sprite->y = e->heightLow;
            e->state = 1;
            e->handlers[0] = NamedObject_TryCapturePlayer;
            e->handlers[1] = NamedObject_ClampTetherAtBarrier;
            e->handlers[2] = NamedObject_UpdateTerrainRegistration;
            sequence = 1;
            AnimData_SelectPaletteBank(e->anim, 0x240);
            m = Script_GetKeywordValue('m', 0);
            if (m)
                e->fieldE0 = m;
            e->row = 0;
            break;
        }
        case 3: {
            s32 m;
            sprite->y = e->heightLow;
            e->state = 1;
            e->handlers[0] = NamedObject_TryCapturePlayer;
            e->handlers[1] = NamedObject_ClampTetherAtBarrier;
            e->handlers[2] = NamedObject_UpdateTerrainRegistration;
            sequence = 1;
            AnimData_SelectPaletteBank(e->anim, 0x246);
            m = Script_GetKeywordValue('m', 0);
            if (m)
                e->fieldE0 = m;
            e->row = 0;
            break;
        }
        case 1: {
            s32 m;
            sprite->y = e->heightLow;
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
                e->fieldE0 = m;
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
            SelectPlatformPalette(e);
            e->fieldE0 = Script_GetKeywordValue('m', 1);
            e->fieldC0 = Script_GetKeywordValue('w', 0x78);
            if (Script_GetKeywordValue('M', 0))
                e->fieldBC = e->fieldC0;
            break;
    }
    Sprite_Init(sprite, e->anim, 0);
    Anim_SetSequence(&e->sequence, manager->sequences, (u16)sequence, e->row & 0xFF, 0);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &e->sequence);
    sprite->flags &= ~1;
    sprite->field7 = 2;
    {
        struct NamedObjectBody *body = &e->body;
        SceneBody_Init(body, e->index + 0xD0E9, &sprite->x, 0, 7, e);
        SceneBody_SetSprite(body, sprite);
        SceneBody_StartRoute(body, e->fieldF4, e->fieldF0, e->fieldF1, e->fieldF2);
        body->field1C = 0;
    }
    sprite->x -= 0x100;
    if (Script_GetKeywordValue('d', 0))
        SetFlagComponent(&e->flags, 0x80);
    if (Script_GetKeywordValue('F', 0))
        SetFlagComponent(&e->flags, 0x1000);
    e->fieldBA = 0;
    e->fieldC2 = 0;
    follow = Script_GetKeywordValue('f', 0);
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
        NamedObject_ClearAttachmentFlag(manager->field1370, 0);
        manager->field1370 = e->name;
        Player_AddAttachmentFlags((*(u8 **)gPlayers), 1, e->name);
        SetFlagComponent(&e->flags, 8);
        {
            u8 *p = *(u8 **)gPlayers + 0x701;
            u32 v = *p;
            u32 m = 1;
            m |= v;
            *p = m;
        }
        cell = cellIndexAt((struct Pos16 *)&e->body.x);
        TerrainModifiers_RegisterCell(e->body.field48, cell, 0, e->body.height >> 8, 0xFF, 4);
        SetFlagComponent(&e->flags, 1);
        Player_SetPositionAndSave((*(u8 **)gPlayers), &e->body.x);
        if (follow == 2) {
            TetherActor_SetPosition(&e->body.x);
            NamedObject_ClearAttachmentFlag(manager->field1374, 1);
            manager->field1374 = e->name;
            TetherActor_AddStatusFlags(3, e->name);
            SetFlagComponent(&e->flags, 0x10);
        }
    }
    NamedObject_SnapTetherToPlatform(manager, e);
    e->fieldBE = Script_GetKeywordValue('s', 6);
    switch (e->kind) {
        case 0:
        case 1:
        case 3:
            if (NamedObject_IsWithinSpriteBounds(e->fieldF4, &e->body.x, (s16)e->fieldBE,
                                                 (s16)e->fieldBE))
                e->state = 3;
            break;
    }
    e->fieldBB = 0;
}
