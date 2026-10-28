#include "global.h"
#ifdef NONMATCHING
// fuzzy: 58.1%
// Full initialization draft; remaining differences are address hoisting,
// helper boundaries, stack layout and animation register allocation.
#include "scene_actor_control_motion.h"
#include "scene_actor_terrain.h"
#include "render.h"
#include "player.h"
#define U8(p, n)  (*((u8 *)(p) + (n)))
#define U16(p, n) (*(u16 *)((u8 *)(p) + (n)))
#define S16(p, n) (*(s16 *)((u8 *)(p) + (n)))
#define U32(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define PTR(p, n) (*(void **)((u8 *)(p) + (n)))
#define POS(p, n) (*(struct ContactPosition *)((u8 *)(p) + (n)))
void CpuSet(const void *, void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, u32, void *);
void Ghost_InitActorProfile(void *);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);
void SceneBody_Init(void *, u32, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, u32, u32);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
s32 Route_GetCurrentPointFacing(void *);
s32 SceneActorScript_GetManagerField62(void);
void SceneActor_InitLinkedReactionColliders(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 Region_FindContaining(void *);
void SceneActorPlacement_HideActor(void *, u32);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void Ghost_InitMotionPreset(void *);
void Ghost_InitActionTable(void *);
void Ghost_InitReactionTable(void *);
void Ghost_InitEffectTable(void *);
void Ghost_GetDirectionalFrame(u16, s32 *, s32 *);
void SceneActor_InitLinkedSprites(void *);
u8 SceneActorRegistry_LinkActor(void *);
void SceneActorControl_OnIdle(void);
void Ghost_OnBehaviorUpdate(void);
void Ghost_GetBehaviorCode(void);
void Ghost_TurnTowardHeading(void);
void Ghost_UpdateDirectionalPose(void);
void Ghost_UpdateDirectionalPoseB(void);
void Ghost_UpdateHitStagger(void);
void Ghost_UpdateDefeat(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void Ghost_ResetMovementState(void);
void Ghost_OnControlCleanup(void);
void Ghost_GetControlStatus(void);
void SceneActorScript_OnCommand(void);
void Ghost_OnActorUpdate(void);
void Ghost_Destroy(void);
extern const u8 gUnk_08605E14[];
struct GhostRoutePoint {
    s16 x, z;
};
struct GhostRouteLink {
    u8 unknown0[8];
    struct GhostRoutePoint *point;
};
struct ResourceCopy {
    u32 words[8];
};
static inline s32 Value(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Cell(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 Height(u32 cell, struct ContactPosition *p)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    u32 shape, height;
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto z;
    if (shape == 2)
        goto x;
    goto done;
z:
    height -= U8(p, 4);
    goto done;
x:
    height -= U8(p, 0);
done:
    return height;
}
static inline void DrawablePosition(struct SceneActorRegistryResetView *a)
{
    if (!a->drawableKind)
        POS(a->drawable, 0x1C) = a->position;
    else
        POS(a->drawable, 0x40) = a->position;
}
static inline void StepSequence(void *obj)
{
    struct SceneActorControlMotionDrawable *owner = obj;
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    state = &owner->sequence;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = owner->sequence.commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        u32 input = state->flags;
        u32 first;
        u32 maskBit = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        u32 first;
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        u32 temp;
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                } else {
                    state->index = index - 1;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                } else {
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                s32 weight = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}

struct GhostAttackFields {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
static inline void InitializeAttackFields(struct SceneActorRegistryResetView *a)
{
    struct GhostAttackFields *fields = (void *)((u8 *)a + 0xB4);
    s32 i;
    if (!Script_SeekToKeyword('a'))
        return;
    for (i = 0; i < 4; i++) {
        fields->first[i] = Value(24);
        fields->second[i] = Value(24);
        fields->flags[i] = Value(0);
        fields->parameter[i] = Value(0);
    }
}
struct GhostTargeting {
    u32 target;
    u8 unknown4[10];
    u8 heading, flags;
    u16 x, z;
    u32 distance, range, limit, zSquared;
};
static inline void InitializeScriptFields(struct SceneActorRegistryResetView *a, u32 *fill)
{
    u32 id;
    struct ContactPosition origin;
    s32 i, first, second;
    struct GhostTargeting *motion = (void *)&a->fieldDC;
    a->flags2CC = 0;
    a->flags2D0 = 0;
    a->enabled = 0;
    U16(a, 0x2E8) = 8;
    a->flags2C0 = Script_GetKeywordValue('M', 0);
    *fill = 0;
    CpuSet(fill, &origin, 0x05000002);
    id = Script_GetKeywordValue('N', 0);
    InitializeAttackFields(a);
    {
        u32 x, z;
        u8 angle;
        if (Script_SeekToKeyword('e')) {
            x = Value(0x30);
            angle = Value(0x40);
            z = x;
        } else {
            x = 0x30;
            z = 0x30;
            angle = 0x40;
        }
        motion->target = 0;
        motion->x = x;
        motion->z = z;
        motion->limit = x * x;
        motion->zSquared = z * z;
        motion->heading = angle;
        if (Test(motion->flags, 4))
            motion->range = motion->limit;
        else
            motion->range = Div(motion->limit * 7, 10);
    }
    if (Script_SeekToKeyword('l')) {
        first = Value(300);
        second = Value(300);
    } else {
        first = 0;
        second = 0;
    }
    if (Script_GetKeywordValue('b', 0))
        a->flags2D0 |= 0x200;
    U32(a, 0x1F0) = (u16)Script_GetKeywordValue('F', 0);
    U32(a, 0x1EC) = (u16)Script_GetKeywordValue('R', 0);
    if (Script_SeekToKeyword('A')) {
        a->defaultHealth = Value(0);
        U8(a, 0x9E) = Value(0);
        U8(a, 0x9F) = Value(0);
        U8(a, 0xA0) = Value(0);
        U16(a, 0xA2) = Value(0);
        U16(a, 0xA4) = Value(0);
    }
    a->flags2D4 = 0;
    a->target = SceneActorControl_OnIdle;
    a->field2B2 = 1;
    a->field2B0 = 0;
    {
        s32 health = (u16)a->defaultHealth;
        if ((s16)health <= 0)
            health = 0;
        a->health = health;
    }
    a->fieldAA = 0;
    if (second <= 0)
        second = first;
    U16(a, 0xA6) = first;
    a->fieldA8 = second;
    U16(a, 0x2FA) = 0;
    if (Script_SeekToKeyword('X')) {
        U32(a, 0xAC) = Value(0);
        U8(a, 0xA1) = Value(0);
    }
    U16(a, 0xB2) = Script_GetKeywordValue('K', 2);
    i = (s16)Script_GetKeywordValue('V', -1);
    if (i < 0)
        i = 0xFFFF;
    U16(a, 0x9A) = i;
    if (Script_GetKeywordValue('H', 0))
        a->flags2D0 |= 0x800;
    SceneBody_Init((u8 *)a + 0x50, id, &origin, 0, 4, a);
    SceneBody_SetSprite((u8 *)a + 0x50, a->drawableKind ? 0 : a->drawable);
    SceneBody_SetMovementProbe((u8 *)a + 0x50, (u8 *)a + 0x40, 0x40, 0x40);
}
static inline void InstallPrimaryCallbacks(struct SceneActorRegistryResetView *a, void *cb274,
                                           void *cb260, void *cb224, void *cb228, void *cb22C,
                                           void *cb230, void *cb234)
{
    void *defaultReaction = SceneActorReaction_OnEnemyDefault;
    u32 zero = 0;
    u32 mask = 0x2000;
    a->flags2CC |= mask;
    PTR(a, 0x274) = cb274;
    PTR(a, 0x260) = cb260;
    a->entryTarget224 = cb224;
    a->entryTarget228 = cb228;
    a->savedTarget = cb22C;
    a->reactionTarget230 = cb230;
    PTR(a, 0x234) = cb234;
    PTR(a, 0x220) = 0;
    PTR(a, 0x278) = defaultReaction;
    PTR(a, 0x270) = 0;
    PTR(a, 0x248) = 0;
    PTR(a, 0x26C) = 0;
}
static inline void InitializeCallbacks(struct SceneActorRegistryResetView *a)
{
    SceneActor_InitLinkedReactionColliders(a);
    InstallPrimaryCallbacks(a, Ghost_OnBehaviorUpdate, Ghost_GetBehaviorCode,
                            Ghost_TurnTowardHeading, Ghost_UpdateDirectionalPose,
                            Ghost_UpdateDirectionalPoseB, Ghost_UpdateHitStagger,
                            Ghost_UpdateDefeat);
    PTR(a, 0x238) = SceneActorReaction_SinkAndRemove;
    PTR(a, 0x264) = SceneActorReaction_ResolveDamage;
    PTR(a, 0x268) = SceneActorPlacement_PlayResponseAndResetState;
    PTR(a, 0x240) = 0;
    a->word104 = 0;
    a->word108 = 0;
    a->word10C = 0;
    a->word110 = 0;
    a->flags114 = 0;
    PTR(a, 0x248) = Ghost_ResetMovementState;
    PTR(a, 0x23C) = Ghost_OnControlCleanup;
    PTR(a, 0x270) = Ghost_GetControlStatus;
    PTR(a, 0x250) = (void *)gUnk_08605E14;
    a->flags2C0 |= 0x20;
    a->flags2CC |= 0x200;
    a->flags2D0 |= 0x02100400;
    SceneActorBehavior_InitializeShadow(a);
    SceneActorTerrain_ReadScriptParameters(a);
    {
        u32 disabled = 0;
        if (Test(a->flags2C0, 4)) {
            a->flags2D0 |= 0x2000;
            disabled = 1;
        }
        if (disabled != 1) {
            a->state2A8 = 0;
            a->phase2A9 = 0;
            a->behavior2AA = 0;
            a->counter2AC = 0;
            a->field2B1 = 1;
            a->field2B2 = 1;
            a->field2B0 = 0;
            a->field2B5 = 0;
            a->target = a->savedTarget;
            a->field2C8 = 0;
            a->flags114 &= ~1;
            a->word104 = 0;
        }
    }
    if (Test(a->flags2C0, 2))
        U16(a, 0x2FA) = U32(gGameState, 0x5A0);
    else
        U16(a, 0x2FA) = Region_FindContaining(&a->position);
    if (Test(a->flags2C0, 0x40)) {
        struct ContactCollider *collider = &a->colliderA;
        u32 mask = ~0x100;
        collider->flags = mask & collider->flags;
    }
}
s32 Ghost_Init(struct SceneActorRegistryResetView *a)
{
    u32 fill;
    struct ContactPosition zero, point;
    s32 row, frameFlags;
    u8 *route;
    u8 *profile;
    U32(a, 0x558) = 0;
    SceneActorRegistry_SetObjectContext(a, a, 0, 0);
    a->category = 0x17;
    Ghost_InitActorProfile(a);
    if (!a->drawableKind) {
        u8 *d;
        a->drawable = Heap_Alloc(0x5C);
        Memory_Clear(a->drawable, 0x5C);
        d = a->drawable;
        AnimData_Load(d + 0x2C, 0xD637);
        Sprite_Init(d, d + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(d, 0x29FB);
        if (a->category == 0x11 || a->category == 0x17)
            d[7] = 1;
        a->direction = 0;
    } else {
        u8 *d;
        void *res;
        a->drawable = Heap_Alloc(0x80);
        Memory_Clear(a->drawable, 0x80);
        d = a->drawable;
        fill = 0;
        CpuSet(&fill, &zero, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0xD637);
        *(struct ResourceCopy *)d = *(struct ResourceCopy *)res;
        ResourceHeader_Relocate(d, res);
        Widget_InitLinked(d + 0x20, d, 0, 0, 2, 2, 0x3C, &zero);
        U8(a, 0x2B7) = 0xFF;
    }
    InitializeScriptFields(a, &fill);
    route = (u8 *)a + 0x60C;
    route[0xF] = 0;
    if (Script_SeekToKeyword('r')) {
        route[0x10] = Value(0);
        route[0x11] = 0;
        route[0x12] = Value(0);
        route[0xC] = route[0x12];
        route[0xD + route[0xF]] = route[0x10];
        {
            u8 *out = route + 0xD + (route[0xF] ^ 1);
            u32 value;
            if (Script_GetPc())
                value = Script_GetValue();
            else
                value = route[0x10];
            *out = value;
        }
        Path_GetPointPositionById((u16 *)&point, route[0x10], route[0x12]);
        a->terrainCell = Cell(&point);
        a->groundHeight2BE = Height(a->terrainCell, &point);
        point.y = a->groundHeight2BE;
        SceneBody_StartRoute((u8 *)a + 0x50, route, route[0x10], route[0x11], route[0x12]);
        {
            struct ContactPosition *dest = &a->position;
            struct GhostRouteLink *link = (void *)route;
            dest->x = link->point->x;
            dest->z = link->point->z;
            dest->y = Height(Cell(dest), dest);
        }
        DrawablePosition(a);
        a->direction = Route_GetCurrentPointFacing(route);
    } else {
        route[0x10] = 0;
        route[0x11] = 0;
        route[0x12] = 0;
        route[0xC] = 0;
        route[0xD + route[0xF]] = 0;
        route[0xD + (route[0xF] ^ 1)] = 0;
    }
    if (Script_SeekToKeyword('p')) {
        a->position.x = Value(0);
        a->position.y = Value(0);
        a->position.z = Value(0);
        a->terrainCell = Cell(&a->position);
        a->groundHeight2BE = Height(a->terrainCell, &a->position);
        U16(a, 0x564) = a->groundHeight2BE;
        DrawablePosition(a);
    } else
        U16(a, 0x564) = a->groundHeight2BE;
    U16(a, 0x622) = Script_GetKeywordValue('l', 0);
    POS(a, 0x604) = POS(gPlayers[0], 0x30);
    if (Script_SeekToKeyword('P')) {
        U8(a, 0x560) = Value(0);
        U8(a, 0x561) = Value(0);
        U8(a, 0x5BC) = Value(0);
    }
    if (Script_GetKeywordValue('g', 0))
        U32(a, 0x558) |= 0x100;
    if (Script_SeekToKeyword('d')) {
        U16(a, 0x5C0) = Value(0);
        U32(a, 0x5C4) = Value(0);
        U32(a, 0x5C4) *= U32(a, 0x5C4);
    } else {
        U16(a, 0x5C0) = 0;
        U32(a, 0x5C4) = 0;
    }
    if (Script_GetKeywordValue('i', 0))
        U32(a, 0x558) |= 0x200;
    a->paletteOffset = Script_GetKeywordValue('C', 0);
    a->basePaletteBank = 0x1EB;
    U16(a, 0x1E2) = 0x192;
    U16(a, 0x1E4) = a->paletteOffset + 0x1EB;
    if (!a->drawableKind)
        AnimData_SelectPaletteBank((u8 *)a->drawable + 0x2C, a->basePaletteBank + a->paletteOffset);
    else
        PTR(a->drawable, 0x68) = gPaletteBankData + (a->basePaletteBank + a->paletteOffset) * 32;
    if (a->drawableKind == 1)
        U16(a->drawable, 0x5A) = SceneActorScript_GetManagerField62() - 0x5FFF;
    U16(a, 0x1DC) = a->basePaletteBank;
    a->paletteBank = a->basePaletteBank + 2;
    a->field1EA = 0x1EE;
    if (!Script_GetKeywordValue('U', 0)) {
        u8 *context = a->context;
        s32 enabled;
        if (Test(U32(context, 0x558), 0x100))
            enabled = Test(U32(gGameState, 0x868), 0x20);
        else
            enabled = S16(gGameState, 0x876);
        if (enabled > 0) {
            U32(context, 0x558) |= 0x40;
            U32(a->drawable, 0) |= 1;
        } else {
            U32(context, 0x558) |= 0x20;
            U32(a->drawable, 0) &= ~1;
        }
        U16(context, 0x5B2) = 0;
    }
    InitializeCallbacks(a);
    if (Script_GetKeywordValue('S', 0) || !Test(U32(gGameState, 0x5A0) == U16(a, 0x2FA), 1))
        SceneActorPlacement_HideActor(a, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    if (!a->drawableKind)
        StepSequence(a->drawable);
    else
        Anim_Step((u8 *)a->drawable + 0x20, a->drawable);
    U32(a, 0x304) = 0;
    SceneActorRegistry_RegisterCategoryReceiver(a);
    SceneActorBehavior_InstallCallbackTables(a);
    Ghost_InitMotionPreset(a);
    Ghost_InitActionTable(a);
    Ghost_InitReactionTable(a);
    if (U16(a, 0x5C0)) {
        Ghost_InitEffectTable(a);
        a->flags2D0 |= 8;
    }
    profile = (u8 *)SceneActorProfiles_GetEntryAddress(a->profileId, a->profileMode, 1);
    Ghost_GetDirectionalFrame(a->direction, &row, &frameFlags);
    {
        u32 selected = *profile;
        u32 force = frameFlags & 0x10;
        frameFlags &= ~0x10;
        if (!a->drawableKind) {
            U8(a, 0x2B7) = selected;
            Anim_SetSequence((u8 *)a->drawable + 0x48, PTR(a->drawable, 0x58), (u16)selected,
                             (u8)row, (u8)frameFlags);
        } else {
            u8 *tile = (u8 *)a->drawable + 0x20;
            u32 mode;
            selected += row;
            if (!force && U8(a, 0x2B7) == selected && !a->field2B3) {
                if (frameFlags == 1 && (U32(tile, 8) & 4))
                    goto animationDone;
                if (frameFlags == 0 && !(U32(tile, 8) & 4))
                    goto animationDone;
            }
            U8(a, 0x2B7) = selected;
            mode = 4;
            if (frameFlags & 4)
                mode = 6;
            Anim_SetKeyframes(tile, a->drawable, (u16)selected, 2, mode);
            if (frameFlags & 1)
                U32(tile, 8) |= 4;
            else
                U32(tile, 8) &= ~4;
        }
        a->field2B3 = 0;
    }
animationDone:
    U16(a, 0x2E8) = 2;
    SceneActor_InitLinkedSprites(a);
    a->defaultFlags2C0 = a->flags2C0;
    a->defaultField2EA = a->field2EA;
    a->defaultFlags2CC = a->flags2CC;
    a->defaultFlags2D0 = a->flags2D0;
    a->direction120 = a->direction;
    a->defaultState = a->state2A8;
    a->defaultPhase = a->phase2A9;
    a->defaultBehavior = a->behavior2AA;
    a->defaultControl = a->target;
    a->defaultField2B5 = a->field2B5;
    a->flags130 = U32(a, 0x558);
    PTR(a, 0x24C) = SceneActorScript_OnCommand;
    PTR(a, 0x258) = Ghost_OnActorUpdate;
    PTR(a, 0x25C) = Ghost_Destroy;
    if (SceneActorRegistry_LinkActor(a))
        return 0;
    return -1;
}

#else
INCLUDE_ASM("asm/nonmatching", Ghost_Init);
#endif
