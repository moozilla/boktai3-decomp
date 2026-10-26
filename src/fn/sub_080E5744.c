#include "global.h"
#ifdef NONMATCHING
// fuzzy: 45.8%
// Complete initializer draft. Address hoisting, callback-store ordering,
// script helper scopes and register allocation remain nonmatching.
#include "scene_actor_control_motion.h"
#include "scene_actor_terrain.h"
#include "render.h"
#define U8(p, n)  (*((u8 *)(p) + (n)))
#define U16(p, n) (*(u16 *)((u8 *)(p) + (n)))
#define S16(p, n) (*(s16 *)((u8 *)(p) + (n)))
#define U32(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define PTR(p, n) (*(void **)((u8 *)(p) + (n)))
#define POS(p, n) (*(struct ContactPosition *)((u8 *)(p) + (n)))
void CpuSet(const void *, void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, u32, void *);
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
void SceneActor_InitLinkedSprites(void *);
u8 SceneActorRegistry_LinkActor(void *);
void SceneActorControl_OnIdle(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void SceneActorScript_OnCommand(void);
extern u8 *gUnk_02000580[];
void SceneActor_RefreshLivingCountAndProfile(void *);
void TargetRegistry_RegisterActor(void *, void *, void *);
void SceneActorCollision_InitGateColliders(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void Dog_SetControlPreset(void *);
void Dog_SetPrimaryBehaviorPreset(void *);
void Dog_SetSecondaryBehaviorPreset(void *);
void Dog_ChooseReactionBehavior(void);
void Dog_OnAuxiliaryUpdate(void);
void Dog_OnClearActivityFlag(void);
void Dog_OnReactionCleanup(void);
void Dog_OnSecondaryReactionCleanup(void);
void Dog_ReleaseResources(void);
void Dog_ResetActionState(void);
void Dog_ResolvePendingObject(void);
void Dog_RestoreRoute(void);
void Dog_ResumeAfterReaction(void);
void Dog_SelectAnimationOne(void);
void Dog_SelectAnimationZero(void);
void Dog_Update(void);
void Dog_UpdateDefeatDelay(void);
void Dog_UpdateTargetGate(void);
void Dog_UpdateTurnSequence(void);
void SceneActorControl_BeginHiddenDefeat(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginDefeatBurst(void);
void SceneActorReaction_BeginFadedState(void);
void SceneActorReaction_BeginReactionSound(void);
void SceneActorReaction_BeginTargetRecovery(void);
void SceneActorReaction_RestoreAfterTurns(void);
void SceneActorReaction_ResumeAfterAnimation(void);
void SceneActorReaction_TryResumeRoute(void);
void SceneActorReaction_TurnBackToRoute(void);
void SceneActorReaction_UpdateAnimationGate(void);
extern const u8 gUnk_086108B0[];
void sub_0807DB34(void);
void sub_0807F130(void);
void sub_0807F81C(void);
void sub_0807FCA8(void);
void sub_08080BE0(void);
void sub_0808170C(void);
void sub_080DE6E0(void);
void sub_080DFC34(void);
void sub_080E0750(void);
void sub_081363F4(void);
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
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 Height(u32 cell, struct ContactPosition *p)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    u32 shape, height;
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + cell * 4 + 12;
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

static inline void InitializeCallbacks(struct SceneActorRegistryResetView *a)
{
    u32 option;
    SceneActorCollision_InitGateColliders(a);
    a->flags2CC |= 0x2000;
    PTR(a, 0x274) = (void *)Dog_Update;
    PTR(a, 0x260) = (void *)sub_080DE6E0;
    PTR(a, 0x224) = (void *)sub_080DFC34;
    PTR(a, 0x228) = (void *)Dog_SelectAnimationOne;
    PTR(a, 0x22C) = (void *)Dog_SelectAnimationZero;
    PTR(a, 0x230) = (void *)sub_080E0750;
    PTR(a, 0x234) = (void *)SceneActorControl_BeginHiddenDefeat;
    PTR(a, 0x220) = (void *)Dog_OnReactionCleanup;
    PTR(a, 0x278) = (void *)SceneActorReaction_OnEnemyDefault;
    PTR(a, 0x270) = (void *)sub_0807F81C;
    PTR(a, 0x250) = (void *)gUnk_086108B0;
    PTR(a, 0x26C) = (void *)Dog_ResolvePendingObject;
    PTR(a, 0x238) = (void *)SceneActorReaction_SinkAndRemove;
    PTR(a, 0x264) = (void *)sub_081363F4;
    PTR(a, 0x268) = (void *)SceneActorPlacement_ResetStateThree;
    PTR(a, 0x248) = 0;
    PTR(a, 0x240) = 0;
    a->word104 = 0;
    a->word108 = 0;
    a->word10C = 0;
    a->word110 = 0;
    a->flags114 = 0;
    PTR(a, 0x5D0) = (u8 *)a + 0x6AC;
    PTR(a, 0x5D4) = (u8 *)a + 0x6D0;
    PTR(a, 0x5F4) = (void *)Dog_UpdateDefeatDelay;
    PTR(a, 0x5F8) = (void *)SceneActorReaction_BeginTargetRecovery;
    PTR(a, 0x5FC) = (void *)SceneActorReaction_BeginFadedState;
    PTR(a, 0x600) = (void *)SceneActorReaction_BeginDefeatBurst;
    PTR(a, 0x43C) = (void *)Dog_ResumeAfterReaction;
    PTR(a, 0x618) = (void *)sub_0807DB34;
    PTR(a, 0x5F0) = (void *)SceneActorReaction_RestoreAfterTurns;
    PTR(a, 0x60C) = (void *)sub_0807FCA8;
    PTR(a, 0x604) = (void *)Dog_OnSecondaryReactionCleanup;
    PTR(a, 0x614) = (void *)SceneActorReaction_TurnBackToRoute;
    PTR(a, 0x444) = (void *)sub_0807F130;
    PTR(a, 0x41C) = (void *)SceneActorReaction_UpdateAnimationGate;
    PTR(a, 0x5DC) = (void *)sub_0808170C;
    PTR(a, 0x5E0) = (void *)SceneActorReaction_ResumeAfterAnimation;
    PTR(a, 0x5E4) = (void *)SceneActorReaction_TryResumeRoute;
    PTR(a, 0x610) = (void *)SceneActorReaction_BeginReactionSound;
    U32(a, 0x440) = 0;
    U8(a, 0x42B) = 1;
    PTR(a, 0x608) = 0;
    PTR(a, 0x61C) = 0;
    PTR(a, 0x620) = 0;
    PTR(a, 0x5E8) = 0;
    U16(a, 0x41A) = 40;
    ProximityVolume_TranslateBounds((u8 *)a + 0x6AC, &a->position);
    SceneActorMovement_ResolveRoutePosition(a);
    PTR(a, 0x248) = Dog_ResetActionState;
    PTR(a, 0x23C) = Dog_UpdateTargetGate;
    PTR(a, 0x268) = Dog_ChooseReactionBehavior;
    PTR(a, 0x61C) = Dog_OnClearActivityFlag;
    PTR(a, 0x5EC) = Dog_RestoreRoute;
    PTR(a, 0x5F0) = Dog_UpdateTurnSequence;
    U16(a, 0x41A) = 20;
    U16(a, 0x2E8) = 0;
    a->flagsEB |= 1;
    a->flags2CC |= 0x01600000;
    a->flags2D0 |= 8;
    SceneActorBehavior_InitializeShadow(a);
    SceneActorTerrain_ReadScriptParameters(a);
    option = Script_GetKeywordValue('u', 0);
    U16(a, 0x5CC) = 0x21;
    if (option) {
        a->flags2CC |= 0x40000;
        if (option == 2)
            a->flags2CC |= 0x80000;
    }
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
        U16(a, 0x2FA) = U32(gUnk_02000710, 0x5A0);
    else
        U16(a, 0x2FA) = Region_FindContaining(&a->position);
    if (Test(a->flags2C0, 0x40))
        U16(a, 0x13A) &= ~0x100;
}
// confidence=low: Dog_Create calls this drawable, script, route and behavior initializer.
#define Dog_Init sub_080E5744
s32 Dog_Init(struct SceneActorRegistryResetView *a)
{
    u32 fill;
    struct ContactPosition zero, origin, point;
    s32 row, frameFlags;
    u32 id;
    s32 i, first, second;
    s32 originalSecond;
    u8 *route;
    u8 *profile;
    U32(a, 0x678) = 0;
    U8(a, 0x6A6) = 0;
    SceneActorRegistry_SetObjectContext(a, a, 0, (void *)1);
    a->category = 0x15;
    SceneActor_RefreshLivingCountAndProfile(a);
    if (!a->drawableKind) {
        u8 *d;
        a->drawable = Heap_Alloc(0x5C);
        Memory_Clear(a->drawable, 0x5C);
        d = a->drawable;
        AnimData_Load(d + 0x2C, 0xD635);
        Sprite_Init(d, d + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(d, 0xF6E7);
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
        res = ScriptProc_Start(0xCB05, 0xD635);
        *(struct ResourceCopy *)d = *(struct ResourceCopy *)res;
        ResourceHeader_Relocate(d, res);
        Widget_InitLinked(d + 0x20, d, 0, 0, 2, 2, 0x3C, &zero);
        U8(a, 0x2B7) = 0xFF;
    }
    {
        u32 fill, id;
        struct ContactPosition origin;
        s32 i, first, second, third;
        s32 savedSecond;
        a->flags2CC = 0;
        a->flags2D0 = 0;
        a->enabled = 0;
        U16(a, 0x2E8) = 8;
        a->flags2C0 = Script_GetKeywordValue('M', 0);
        a->field2C2 = 0x40;
        fill = 0;
        CpuSet(&fill, &origin, 0x05000002);
        id = Script_GetKeywordValue('N', 0);
        if (Script_SeekToKeyword('a')) {
            for (i = 0; i < 4; i++) {
                U16(a, 0xB4 + i * 2) = Value(24);
                U16(a, 0xBC + i * 2) = Value(24);
                U32(a, 0xCC + i * 4) = Value(0);
                U16(a, 0xC4 + i * 2) = Value(0);
            }
        }
        if (Script_SeekToKeyword('l')) {
            first = Value(300);
            second = Value(300);
            third = Value(300);
        } else {
            first = 0;
            second = 0;
            third = 0;
        }
        savedSecond = second;
        U16(a, 0x414) = 0;
        U16(a, 0x416) = third;
        U16(a, 0x418) = 0;
        U16(a, 0x41A) = 0;
        U32(a, 0x41C) = 0;
        {
            s32 radius;
            if (Script_SeekToKeyword('n')) {
                radius = Value(0);
                U16(a, 0x420) = Value(0);
            } else {
                radius = 0;
                U16(a, 0x420) = 0;
            }
            U16(a, 0x422) = radius;
            U32(a, 0x424) = U16(a, 0x422) * U16(a, 0x422);
            U8(a, 0x42A) = 0;
        }
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
            a->fieldDC = 0;
            U16(a, 0xEC) = x;
            U16(a, 0xEE) = z;
            a->distanceLimitF8 = x * x;
            U32(a, 0xFC) = z * z;
            U8(a, 0xEA) = angle;
            if (Test(a->flagsEB, 4))
                a->rangeF4 = a->distanceLimitF8;
            else
                a->rangeF4 = Div(a->distanceLimitF8 * 7, 10);
        }
        if (Script_GetKeywordValue('b', 0))
            a->flags2D0 |= 0x200;
        U32(a, 0x1F0) = (u16)Script_GetKeywordValue('F', 0);
        U32(a, 0x1EC) = (u16)Script_GetKeywordValue('R', 0);
        if (Script_SeekToKeyword('A')) {
            u32 health = (u16)Value(0);
            a->defaultHealth = health;
            U16(a, 0x5B6) = ((s32)health < 0 ? -(-(s32)health / 2) : (s32)health / 2);
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
        a->health = a->defaultHealth > 0 ? a->defaultHealth : 0;
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
        if (Script_GetKeywordValue('L', 0))
            a->flags2D0 |= 0x8000;
        SceneBody_Init((u8 *)a + 0x50, id, &origin, 0, 4, a);
        originalSecond = savedSecond;
    }
    route = (u8 *)a + 0x448;
    route[0xF] = 0;
    if (Script_SeekToKeyword('r')) {
        route[0x10] = Value(0);
        route[0x11] = 0;
        route[0x12] = Value(0);
        route[0xC] = route[0x12];
        route[0xD + route[0xF]] = route[0x10];
        route[0xD + (route[0xF] ^ 1)] = Value(route[0x10]);
        Path_GetPointPositionById((u16 *)&point, route[0x10], route[0x12]);
        a->terrainCell = Cell(&point);
        a->groundHeight2BE = Height(a->terrainCell, &point);
        point.y = a->groundHeight2BE;
        SceneBody_StartRoute((u8 *)a + 0x50, route, route[0x10], route[0x11], route[0x12]);
        a->position.x = ((u16 *)PTR(route, 8))[0];
        a->position.z = ((u16 *)PTR(route, 8))[1];
        a->position.y = Height(Cell(&a->position), &a->position);
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
    if (Test(a->flags2C0, 0x200))
        TargetRegistry_RegisterActor((u8 *)a + 0x404, (u8 *)a + 0x50, sub_08080BE0);
    SceneBody_SetSprite((u8 *)a + 0x50, a->drawableKind ? 0 : a->drawable);
    if (a->category == 0x15)
        SceneBody_SetMovementProbe((u8 *)a + 0x50, (u8 *)a + 0x40, 60, 60);
    else
        SceneBody_SetMovementProbe((u8 *)a + 0x50, (u8 *)a + 0x40, 64, 64);
    U16(a, 0x3D4) = a->id;
    U32(a, 0x3D8) = originalSecond;
    if (Script_SeekToKeyword('P')) {
        U16(a, 0x69C) = Value(200);
        U16(a, 0x69E) = Value(320);
    }
    if (Script_SeekToKeyword('Y')) {
        U32(a, 0x684) = Value(0);
        U32(a, 0x684) *= U32(a, 0x684);
        U16(a, 0x692) = Value(640);
        U16(a, 0x694) = Value(640);
    }
    U8(a, 0x6A9) = Script_GetKeywordValue('d', 50);
    if (!U8(a, 0x6A9))
        U8(a, 0x6A9) = 50;
    U16(a, 0x6A0) = 0xFFFF;
    U16(a, 0x6A4) = 0;
    a->paletteOffset = Script_GetKeywordValue('C', 0);
    a->basePaletteBank = 0x273;
    U16(a, 0x1E2) = 0x192;
    U16(a, 0x1E4) = a->paletteOffset + 0x273;
    if (!a->drawableKind)
        AnimData_SelectPaletteBank((u8 *)a->drawable + 0x2C, a->basePaletteBank + a->paletteOffset);
    else
        PTR(a->drawable, 0x68) = gPaletteBankData + (a->basePaletteBank + a->paletteOffset) * 32;
    if (a->drawableKind == 1)
        U16(a->drawable, 0x5A) = SceneActorScript_GetManagerField62() - 0x5FFF;
    InitializeCallbacks(a);
    if (Script_GetKeywordValue('S', 0) || !Test(U32(gUnk_02000710, 0x5A0) == U16(a, 0x2FA), 1))
        SceneActorPlacement_HideActor(a, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    if (!a->drawableKind)
        StepSequence(a->drawable);
    else
        Anim_Step((u8 *)a->drawable + 0x20, a->drawable);
    U32(a, 0x304) = 0;
    {
        u8 *context = a->context;
        if (a->paletteOffset == 0 || a->paletteOffset == 2)
            U32(context, 0x67C) = 0x40000;
        else {
            U32(context, 0x67C) = 0x40000;
            U32(context, 0x680) = 0x38400;
        }
        U32(context, 0x688) = 0xFFFF;
        U32(context, 0x68C) = U32(context, 0x67C);
    }
    SceneActorBehavior_InstallCallbackTables(a);
    Dog_SetControlPreset(a);
    Dog_SetPrimaryBehaviorPreset(a);
    Dog_SetSecondaryBehaviorPreset(a);
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
    PTR(a, 0x24C) = SceneActorScript_OnCommand;
    PTR(a, 0x258) = Dog_OnAuxiliaryUpdate;
    PTR(a, 0x25C) = Dog_ReleaseResources;
    if (SceneActorRegistry_LinkActor(a))
        return 0;
    return -1;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080E5744);
#endif
