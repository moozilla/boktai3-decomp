#include "global.h"
#include "render.h"
#include "engine_globals.h"
typedef void (*MonsterInitCommandHandler)(void *actor, const u16 *command);
s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void CpuSet(const void *, void *, u32);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void Bee_InitProfile(void *);
void SceneActorControl_OnIdle(void);
void Bee_OnUpdate(void);
void sub_08093094(void);
void Bee_UpdateTurnAnimation(void);
void Bee_ResetIdleAnimation(void);
void Bee_UpdateMovementStop(void);
void Bee_UpdateDefeat(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void SceneActorReaction_SinkAndRemove(void);
void Bee_ResetRoute(void);
void Slime_SetStateFlag(void);
void Bee_OnDetectionCheck(void);
void SceneActorScript_OnCommand(void);
void Bee_OnPlayerInteraction(void);
void Bee_Destroy(void);
void *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 SceneActorScript_GetManagerField62(void);
void Bee_InitColliders(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void Bee_InitActionPreset(void *);
void Bee_InitBehaviorHandlers(void *);
void Bee_InitReactionHandlers(void *);
void SceneActorControl_RestartPositionCurve(void *);
void ActorShadow_Disable(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);
s32 Region_FindContaining(void *);

extern MonsterInitCommandHandler const gUnk_08605DB8[];
extern MonsterInitCommandHandler const gUnk_086108B0[];
extern u8 *gGameState;
struct MonsterInit32Block {
    u32 w[8];
};
struct MonsterInitPosition {
    s16 x, y, z, pad;
};
struct MonsterInitMap {
    u32 header;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
struct MonsterBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct MonsterInitPosition position;
    u8 unknown64[0x34];
};
struct MonsterAttackParameters {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct MonsterInitTargeting {
    u32 fieldDC;
    u8 unknownE0[0xA];
    u8 fieldEA;
    u8 fieldEB;
    u16 fieldEC;
    u16 fieldEE;
    u8 unknownF0[0x4];
    u32 fieldF4;
    u32 fieldF8;
    u32 fieldFC;
};
struct MonsterInitSavedControl {
    u32 field118;
    u32 field11C;
    u8 field120;
    u8 field121;
    u8 field122;
    u8 field123;
    void *field124;
    u8 field128;
    u8 unknown129[0x3];
    u16 field12C;
    u16 field12E;
    u32 field130;
};
struct MonsterInitActor {
    u8 unknown0[0x4];
    u8 *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct MonsterBody body;
    u8 category;
    u8 unknown99[0x1];
    u16 field9A;
    u16 field9C;
    u8 field9E;
    u8 field9F;
    u8 fieldA0;
    u8 fieldA1;
    u16 fieldA2;
    u16 fieldA4;
    u16 fieldA6;
    u16 fieldA8;
    u16 fieldAA;
    u32 fieldAC;
    u16 fieldB0;
    u16 fieldB2;
    struct MonsterAttackParameters attacks;
    struct MonsterInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct MonsterInitSavedControl saved;
    u8 unknown134[0x6];
    u16 field13A;
    u8 unknown13C[0x3C];
    u16 field178;
    u8 unknown17A[0x66];
    u16 field1E0;
    u16 field1E2;
    u16 field1E4;
    u16 field1E6;
    u8 unknown1E8[0x4];
    u32 field1EC;
    u32 field1F0;
    u8 unknown1F4[0x28];
    void *field21C;
    void *field220;
    void *field224;
    void *field228;
    void *field22C;
    void *field230;
    void *field234;
    void *field238;
    void *field23C;
    u32 field240;
    u8 unknown244[0x4];
    void *field248;
    void *field24C;
    MonsterInitCommandHandler const *field250;
    u8 unknown254[0x4];
    void *field258;
    void *field25C;
    void *field260;
    void *field264;
    void *field268;
    void *field26C;
    void *field270;
    void *field274;
    void *field278;
    u8 unknown27C[0x2C];
    u8 field2A8;
    u8 field2A9;
    u8 field2AA;
    u8 unknown2AB[0x1];
    u32 field2AC;
    u8 field2B0;
    u8 field2B1;
    u8 field2B2;
    u8 unknown2B3[0x1];
    u8 field2B4;
    u8 field2B5;
    u8 unknown2B6[0x1];
    u8 field2B7;
    u8 unknown2B8[0x6];
    u16 field2BE;
    u16 field2C0;
    u16 field2C2;
    u8 unknown2C4[0x4];
    u16 field2C8;
    u8 unknown2CA[0x2];
    u32 field2CC;
    u32 field2D0;
    u16 field2D4;
    u8 unknown2D6[0x12];
    u16 field2E8;
    u16 field2EA;
    u8 unknown2EC[0x8];
    u16 field2F4;
    u8 unknown2F6[0x4];
    u16 field2FA;
    u8 unknown2FC[0x8];
    u32 field304;
    u8 unknown308[0xCC];
};
struct BeeActor {
    struct MonsterInitActor common;
    u32 flags;
    s16 height;
    u16 ground3DA;
    u8 reset3DC;
    u8 unknown3DD[0x4C];
    u8 parameter429;
    u8 unknown42A[2];
    u16 duration42C;
    u8 unknown42E[0x12];
    u32 control440;
    u8 unknown444[0x44];
    struct MonsterInitPosition initialPosition;
};
static inline s32 MonsterInit_Value(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u8 MonsterInit_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void MonsterInit_ClearPosition(u32 *fill, void *object)
{
    struct MonsterInitPosition *position = object;
    *fill = 0;
    CpuSet(fill, position, 0x05000002);
}
static inline u16 MonsterInit_PositionX(const u16 *pos)
{
    return pos[0];
}
static inline u16 MonsterInit_PositionZ(const u16 *pos)
{
    return pos[2];
}
static inline u16 MonsterInit_Cell(u16 *pos)
{
    s32 x = (s16)MonsterInit_PositionX(pos) >> 8;
    s32 z = (s16)MonsterInit_PositionZ(pos) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct MonsterInitMap *)gTerrainMap)->rows[z] + x;
}
static inline u32 MonsterInit_MaskValue(u32 value, u32 mask)
{
    mask &= value;
    return mask;
}
static inline s32 MonsterInit_Height(s32 cell, u16 *pos)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    u32 attributes, shape;
    s32 height;
    if (p)
        p += 4;
    else {
        struct MonsterInitMap *map = (void *)gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    attributes = *p;
    shape = attributes >> 4;
    height = MonsterInit_MaskValue(attributes, 15) << 8;
    switch (shape) {
        case 1:
            height -= ((u8 *)pos)[4];
            break;
        case 2:
            height -= ((u8 *)pos)[0];
            break;
    }
    return height;
}
static inline u8 MonsterInit_GetDrawableKind(struct MonsterInitActor *actor)
{
    return actor->drawableKind;
}
static inline u8 MonsterInit_GetCategory(struct MonsterInitActor *actor)
{
    return actor->category;
}
static inline u16 MonsterInit_GetHealth(struct MonsterInitActor *actor)
{
    return actor->field9C;
}
static inline u16 MonsterInit_GetInitialHealth(struct MonsterInitActor *actor)
{
    return actor->fieldB0;
}
static inline u16 MonsterInit_GetGroundHeight(struct MonsterInitActor *actor)
{
    return actor->field2BE;
}
static inline u16 MonsterInit_GetPaletteBase(struct MonsterInitActor *actor)
{
    return actor->field1E0;
}
static inline u16 MonsterInit_GetAlternatePalette(struct MonsterInitActor *actor)
{
    return actor->field1E2;
}
static inline u16 MonsterInit_GetCurrentPalette(struct MonsterInitActor *actor)
{
    return actor->field1E4;
}
static inline u16 MonsterInit_GetPaletteOffset(struct MonsterInitActor *actor)
{
    return actor->field1E6;
}
#define SV(d) MonsterInit_Value(d)
union MonsterInitSequenceControl {
    u8 raw;
    struct {
        u8 flipX : 1;
        u8 flipY : 1;
        u8 backwards : 1;
        u8 hold : 1;
        u8 unused : 4;
    } __attribute__((packed)) bits;
} __attribute__((packed));
typedef char
    MonsterInitSequenceControlSizeCheck[(sizeof(union MonsterInitSequenceControl) == 1) ? 1 : -1];
struct MonsterInitSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct MonsterInitDrawable {
    u32 flags;
    u8 unknown4[12];
    u16 frame;
    u8 unknown12[0x36];
    struct MonsterInitSequence sequence;
};
static inline u16 MonsterInit_SequenceIndex(struct MonsterInitSequence *state)
{
    return state->index;
}
static inline u8 MonsterInit_SequenceFlags(struct MonsterInitSequence *state)
{
    return state->flags;
}
static inline u16 MonsterInit_SequenceElapsed(struct MonsterInitSequence *state)
{
    return state->elapsed;
}
static inline u8 MonsterInit_SequenceDuration(struct MonsterInitSequence *state)
{
    return state->duration;
}
static inline u8 MonsterInit_SequenceFrameCount(struct MonsterInitSequence *state)
{
    return state->frameCount;
}
static inline u8 MonsterInit_SequenceFrameWeight(struct MonsterInitSequence *state)
{
    return state->frameWeight;
}
static inline u16 MonsterInit_SequenceSpeed(struct MonsterInitSequence *state)
{
    return state->speed;
}
static inline u8 MonsterInit_SequenceMaskedFlags(struct MonsterInitSequence *state, u32 mask)
{
    return state->flags & mask;
}
static inline u32 MonsterInit_ReadSequenceCommand(const u16 *command)
{
    return *command;
}
static inline void StepSequence(void *obj)
{
    struct MonsterInitDrawable *owner = obj;
    struct MonsterInitSequence *state;
    const u16 *command;
    u32 duration, flags, bit;
    state = &owner->sequence;
    {
        u32 offset = MonsterInit_SequenceIndex(state) << 1;
        const u16 *table;
        table = owner->sequence.commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union MonsterInitSequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union MonsterInitSequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    {
        if (++state->elapsed >= state->duration) {
            state->elapsed = 0;
            if (state->flags & 4) {
                if (state->index == 0) {
                    if (state->flags & 8)
                        state->elapsed = state->duration;
                    else
                        state->index = state->frameCount - 1;
                } else {
                    state->index--;
                }
            } else {
                if (++state->index >= state->frameCount) {
                    if (state->flags & 8) {
                        state->elapsed = state->duration;
                        state->index = state->frameCount - 1;
                    } else {
                        state->index = 0;
                    }
                }
            }
            command = state->commands + state->index;
            state->frameWeight =
                MonsterInit_MaskValue(MonsterInit_ReadSequenceCommand(command), 15);
            state->duration = (state->frameWeight * (s32)state->speed) >> 6;
            if (state->duration == 0)
                state->duration = 1;
        }
    }
}
static inline u16 MonsterInit_GetMotionFlags(struct MonsterInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 MonsterInit_GetHeightOffset(struct MonsterInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 MonsterInit_GetBehaviorFlags(struct MonsterInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 MonsterInit_GetStatusFlags(struct MonsterInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 MonsterInit_GetState(struct MonsterInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 MonsterInit_GetPhase(struct MonsterInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 MonsterInit_GetBehavior(struct MonsterInitActor *actor)
{
    return actor->field2AA;
}
static inline void *MonsterInit_GetCurrentControl(struct MonsterInitActor *actor)
{
    return actor->field21C;
}
static inline u8 MonsterInit_GetControlKind(struct MonsterInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 MonsterInit_GetDirection(struct MonsterInitActor *actor)
{
    return actor->body.direction;
}
static inline u16 MonsterInit_GetTerrainCell(struct MonsterInitActor *actor)
{
    return actor->field2F4;
}
static inline u8 MonsterInit_GetTargetingFlags(struct MonsterInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct MonsterInitSavedControl *
MonsterInit_SavedControlOf(struct MonsterInitActor *actor)
{
    struct MonsterInitSavedControl *saved = &actor->saved;
    return saved;
}
static inline struct MonsterInitPosition *MonsterInit_BodyPosition(void *object)
{
    struct MonsterBody *body = object;
    struct MonsterInitPosition *position = &body->position;
    return position;
}
static inline void MonsterInit_SaveControl(struct MonsterInitSavedControl *saved,
                                           struct MonsterInitActor *actor)
{
    saved->field12E = MonsterInit_GetMotionFlags(actor);
    saved->field12C = MonsterInit_GetHeightOffset(actor);
    saved->field118 = MonsterInit_GetBehaviorFlags(actor);
    saved->field11C = MonsterInit_GetStatusFlags(actor);
    saved->field120 = MonsterInit_GetDirection(actor);
    saved->field121 = MonsterInit_GetState(actor);
    saved->field122 = MonsterInit_GetPhase(actor);
    saved->field123 = MonsterInit_GetBehavior(actor);
    saved->field124 = MonsterInit_GetCurrentControl(actor);
    saved->field128 = MonsterInit_GetControlKind(actor);
}
static inline void MonsterInit_SetStatusFlags(struct MonsterInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}
static inline void MonsterInit_SetMotionFlags(struct MonsterInitActor *actor, u16 mask)
{
    actor->field2C0 |= mask;
}
static inline void *MonsterInit_GetIdleControl(struct MonsterInitActor *actor)
{
    return actor->field22C;
}
static inline u8 MonsterInit_TestMotionFlags(struct MonsterInitActor *actor, u32 mask)
{
    if (MonsterInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}
static inline u8 MonsterInit_TestTargetingFlags(struct MonsterInitTargeting *target, u32 mask)
{
    if (MonsterInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void MonsterInit_ClearControlFlags(struct MonsterInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 MonsterInit_IsInCurrentRegion(struct MonsterInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void MonsterInit_SetControl(struct MonsterInitActor *actor, void *callback,
                                          u32 behavior)
{
    u32 zero = 0;
    u32 one = 1;
    actor->field2B2 = one;
    actor->field2B0 = zero;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = zero;
    MonsterInit_ClearControlFlags(actor, ~1);
    actor->field104 = zero;
}
static inline void MonsterInit_SetState(struct MonsterInitActor *actor, u32 state, u32 phase,
                                        u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}
static inline void MonsterInit_ReadTargeting(struct MonsterInitTargeting *target)
{
    s32 r4v, r2v, r1v;
    if (Script_SeekToKeyword(0x65)) {
        r4v = SV(0x30);
        r2v = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
        r1v = r4v;
    } else {
        r4v = 0x30;
        r1v = 0x30;
        r2v = 0x40;
    }
    target->fieldDC = 0;
    target->fieldEC = r4v;
    target->fieldEE = r1v;
    target->fieldF8 = r4v * r4v;
    target->fieldFC = r1v * r1v;
    target->fieldEA = r2v;
    if (MonsterInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);
}
static inline u8 *MonsterInit_FirstAttackData(struct MonsterInitActor *actor)
{
    return (u8 *)actor->attacks.first;
}
struct MonsterInitCollider {
    u8 header[6];
    u16 flags;
};
static inline void MonsterInit_ClearColliderFlags(struct MonsterInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
struct MonsterInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MonsterInit_SetWidgetPalette(struct MonsterInitWidget *widget, u32 palette)
{
    widget->paletteBank = palette;
}
static inline u8 MonsterInit_ReadByte(u8 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u16 MonsterInit_ReadHalfword(u16 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline void MonsterInit_SetFirstAttack(struct MonsterInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}
static inline struct MonsterInitActor *MonsterInit_ReadAttacks(struct MonsterInitActor *actor)
{
    s32 i;
    if (Script_SeekToKeyword('a')) {
        i = 0;
        while (1) {
            MonsterInit_SetFirstAttack(actor, i, MonsterInit_ReadHalfword(24));
            actor->attacks.second[i] = MonsterInit_ReadHalfword(24);
            actor->attacks.flags[i] = MonsterInit_Value(0);
            actor->attacks.parameter[i] = MonsterInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
        return actor;
    }
    return actor;
}

static inline s32 MonsterInit_ClampHealth(u16 initialHealth)
{
    s32 health = (s16)initialHealth;
    if (health <= 0)
        health = 0;
    return health;
}
static inline void MonsterInit_InitializeBody(struct MonsterInitActor *arg0, u32 *fill)
{
    struct MonsterBody *body = &arg0->body;
    struct MonsterInitPosition origin;
    s32 mode, r4v, r6v, r3v;
    s32 initialHealth, scriptVariant;
    arg0->field2CC = 0;
    arg0->field2D0 = 0;
    arg0->field2B4 = 0;
    arg0->field2E8 = 8;
    arg0->field2C0 = Script_GetKeywordValue(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->targeting);

    if (Script_SeekToKeyword(0x6C)) {
        r6v = MonsterInit_Value(0x12C);
        r4v = MonsterInit_Value(0x12C);
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = (u16)Script_GetKeywordValue(0x46, 0);
    arg0->field1EC = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        arg0->fieldB0 = MonsterInit_ReadHalfword(0);
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    initialHealth = MonsterInit_GetInitialHealth(arg0);
    r3v = r4v;
    arg0->field2D4 = 0;
    arg0->field21C = SceneActorControl_OnIdle;
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = MonsterInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (r3v <= 0)
        r3v = r6v;
    arg0->fieldA6 = r6v;
    arg0->fieldA8 = r3v;
    arg0->field2FA = 0;

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_Value(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    scriptVariant = (s16)Script_GetKeywordValue(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->field9A = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->drawableKind ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);
}
static inline u8 *MonsterInit_Drawable(struct MonsterInitActor *actor)
{
    return actor->drawable;
}
static inline void MonsterInit_SetBehaviorFlags(struct MonsterInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}
static inline void MonsterInit_SetCurrentPalette(struct MonsterInitActor *actor, u32 palette)
{
    actor->field1E4 = palette;
}
static inline void Bee_ReadPalette(struct MonsterInitActor *actor, u16 base, u16 alternate)
{
    s32 r1v;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    MonsterInit_SetCurrentPalette(actor, MonsterInit_GetPaletteOffset(actor) + 0xF4);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette((void *)(actor->drawable + 0x20),
                                     SceneActorScript_GetManagerField62() - 0x5FFF);
}
static inline void MonsterInit_ClearMotion(void *object)
{
    struct MonsterInitActor *actor = object;
    u32 zero = 0;
    actor->field104 = zero;
    actor->field108 = zero;
    actor->field10C = zero;
    actor->field110 = zero;
    actor->field114 = zero;
}
static inline void MonsterInit_InstallPrimary(struct MonsterInitActor *actor, void *cb0, void *cb1,
                                              void *cb2, void *cb3, void *cb4, void *hidden,
                                              void *cb5, void *cb6, void *defaultReaction,
                                              void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->field274 = cb0;
    actor->field260 = cb1;
    actor->field224 = cb2;
    actor->field228 = cb3;
    actor->field22C = cb4;
    actor->field230 = cb5;
    actor->field234 = cb6;
    actor->field220 = hidden;
    actor->field278 = defaultReaction;
    actor->field270 = 0;
    actor->field250 = gUnk_086108B0;
    actor->field248 = 0;
    actor->field26C = targetQuery;
    actor->field238 = SceneActorReaction_SinkAndRemove;
    actor->field264 = damage;
    actor->field268 = contact;
    actor->field240 = 0;
    MonsterInit_ClearMotion(actor);
}
static inline void Bee_InstallCompletionCallbacks(struct MonsterInitActor *actor)
{
    actor->field24C = SceneActorScript_OnCommand;
    actor->field258 = Bee_OnPlayerInteraction;
    actor->field25C = Bee_Destroy;
}

static inline void MonsterInit_SetCategory(struct MonsterInitActor *actor, u8 category)
{
    actor->category = category;
}
static inline void MonsterInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void Bee_InstallRoutes(struct MonsterInitActor *actor)
{
    actor->field250 = gUnk_08605DB8;
    actor->field248 = Bee_ResetRoute;
    actor->field23C = Slime_SetStateFlag;
    actor->field270 = Bee_OnDetectionCheck;
}
static inline void MonsterInit_InitializeReceiver(struct MonsterInitActor *actor)
{
    actor->field304 = 0;
}

static inline void MonsterInit_InitializeRegion(struct MonsterInitActor *actor)
{
    if (MonsterInit_TestMotionFlags(actor, 2))
        actor->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        actor->field2FA = Region_FindContaining(&actor->body.position);
}

s32 Bee_Init(void *object)
{
    struct BeeActor *bee = object;
    struct MonsterInitActor *actor = object;
    u32 zero;
    struct MonsterInitPosition tmp;
    u32 flag;
    s32 r1v;
    bee->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 0x14);
    Bee_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x9506, 0x62C7);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        res = ScriptProc_Start(0xCB05, 0x9506);
        *(struct MonsterInit32Block *)heap = *(struct MonsterInit32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->field2B7 = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        bee->parameter429 = MonsterInit_ReadByte(0x68);
        bee->reset3DC = 0;
        bee->duration42C = MonsterInit_ReadHalfword(100);
    }
    if (Script_SeekToKeyword(0x70)) {
        struct MonsterInitPosition position;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        position = actor->body.position;
        if (MonsterInit_GetDirection(actor) == 0)
            position.x += 0x80;
        else
            position.z += 0x80;
        bee->initialPosition = position;
        actor->field2F4 = MonsterInit_Cell((u16 *)&bee->initialPosition);
        {
            s32 groundHeight = MonsterInit_Height(actor->field2F4, (u16 *)&bee->initialPosition);
            s32 offset;
            u16 *ground = &actor->field2BE;
            *ground = groundHeight;
            offset = (u16)actor->body.position.y - groundHeight;
            actor->field2EA = offset;
            bee->height = offset;
            actor->body.position.y = *ground;
            bee->ground3DA = *ground;
        }
    }
    bee->control440 = 0;
    actor->field178 = Script_GetKeywordValue('n', 0);
    Bee_ReadPalette(actor, 0xF4, 0x192);

    Bee_InitColliders(actor);
    MonsterInit_InstallPrimary(
        actor, Bee_OnUpdate, sub_08093094, 0, Bee_UpdateTurnAnimation, Bee_ResetIdleAnimation, 0,
        Bee_UpdateMovementStop, Bee_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
        SceneActorReaction_ResolveDamage, SceneActorPlacement_PlayResponseAndResetState);
    Bee_InstallRoutes(actor);
    MonsterInit_SetStatusFlags(actor, 0x02100408);
    MonsterInit_SetMotionFlags(actor, 0x60);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(actor, 0, 0, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetIdleControl(actor), 0);
    }
    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        StepSequence(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_InitializeReceiver(actor);
    MonsterInit_SetState(actor, 0, 2, 0);
    MonsterInit_SetControl(actor, actor->field228, 1);
    SceneActorBehavior_InstallCallbackTables(actor);
    Bee_InitActionPreset(actor);
    Bee_InitBehaviorHandlers(actor);
    Bee_InitReactionHandlers(actor);
    actor->field2E8 = 2;
    actor->drawable[7] = 1;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Bee_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
