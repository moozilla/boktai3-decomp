#include "scene_script_object.h"
#include "global.h"
#include "directional_script_args.h"
#include "scene_vec.h"
#include "palette_set.h"
#include "render.h"
#include "engine_globals.h"
#include "contact_actor.h"
#include "player_cue_sequence.h"
#include "engine.h"

/* Tentative scene script object; coordinates and five script arguments are observed here. */

void SceneScriptObject_RunPendingScript(struct SceneScriptObject *object)
{
    s32 values[5];
    struct DirectionalScriptArgs args;
    values[0] = object->caller;
    values[1] = object->state;
    values[2] = object->sprite.position.x;
    values[3] = object->sprite.position.y;
    values[4] = object->sprite.position.z;
    args.count = 5;
    args.values = values;
    Script_Call(object->pendingScript, &args);
    object->pendingScript = 0;
}

extern const s16 gUnk_086149C4[];

extern const s32 gUnk_0824F7A4[15];

void CpuSet(const void *, void *, u32);

void ActorShadow_Disable(void *);

void *memcpy(void *, const void *, u32);

static inline s32 Next(s32 bank)
{
    return bank + 1;
}

static inline void ClearFlags(struct ContactCollider *w, s32 mask)
{
    w->flags &= mask;
}

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline s32 PaletteIndex(s32 state, s32 parameter)
{
    if (state <= 1) {
        if (parameter >= 0)
            return parameter + 0x26B;
        return 0x26B;
    }
    return ((state - 2) >> 1) + 0x26B;
}

void SceneScriptObject_UpdateFade(struct SceneScriptObject *object)
{
    struct ContactCollider *widget;
    s32 songs[15];
    s32 index;
    s32 amount;
    s32 zero;
    u8 *dest;
    if (object->fadeTimer > 0 && (u16)--object->fadeTimer == 0) {
        index = PaletteIndex(object->state, object->parameter);
        object->resource.paletteIndex = index;
        CpuSet(gPaletteBankData + index * 32, object->resource.palette, 0x04000008);
    }
    if (object->holdTimer > 0) {
        struct ContactCollider *w = &object->collider;
        w->flags = 4 | w->flags;
        object->holdTimer--;
        widget = w;
    } else {
        struct ContactCollider *w = &object->collider;
        ClearFlags(w, -5);
        widget = w;
    }
    switch (object->state) {
        case 12:
            object->sprite.position.y += Scale(gUnk_086149C4[*(u8 *)&object->counter] * 2);
            break;
        case 14:
            if (object->fadeTimer == 0) {
                u16 phase;
                s32 bank = 0x271;
                object->resource.paletteIndex = bank;
                phase = object->counter;
                if (phase & 0x40) {
                    s32 m = 0x7f;
                    m &= phase;
                    amount = 0x80 - m;
                } else {
                    s32 m = 0x7f;
                    m &= phase;
                    amount = m;
                }
                dest = gPaletteBankData + bank * 32;
                Palette_InterpolateBanks(object->resource.palette, dest,
                                         gPaletteBankData + Next(bank) * 32, amount, 7);
            }
            break;
    }
    if (object->lifetime > 0)
        goto increment;
    zero = object->fadeTimer;
    if (zero != 0)
        goto increment;
    memcpy(songs, gUnk_0824F7A4, 0x3c);
    Sound_PlaySong(songs[object->state]);
    object->sprite.flags |= 1;
    if (object->state == 12)
        ActorShadow_Disable(object->shadow);
    object->finished = 1;
    object->counter = zero;
    widget->flags |= 4;
    if (object->pendingScript)
        SceneScriptObject_RunPendingScript(object);
    return;
increment:
    object->counter++;
}

void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

static inline s32 Sample(u32 table, u32 index)
{
    return *(const u16 *)(index * 2 + table);
}

static inline s32 AdvanceAndSample(u32 *table, u32 index)
{
    *table += index * 2;
    return *(const u16 *)*table;
}

void SceneScriptObject_UpdateBurst(struct SceneScriptObject *object)
{
    struct ContactPosition velocity;
    u32 index;
    u16 counter;
    if (object->counter == 0) {
        s32 i = 0;
        s16 *statePtr;
        struct ContactPosition *position;
        u32 mask;
        struct ContactPosition *v;
        s16 *parameterPtr;
        s32 zero;
        statePtr = &object->state;
        parameterPtr = &object->parameter;
        position = &object->sprite.position;
        mask = 0x3ff;
        v = &velocity;
        zero = 0;
        do {
            s32 kind = (i & 1) ? (0x400) : (0x200);
            u32 table = 0x0203B400;
            s32 x;
            s32 y;
            s32 z;
            u16 index;
            s32 threshold;
            index = (gRandomIndex + 1) & mask;
            x = (Sample(table, index) >> 4) & 0x1f;
            index = (index + 1) & mask;
            y = (Sample(table, index) >> 4) & 0x1f;
            index = (index + 1) & mask;
            z = (Sample(table, index) >> 4) & 0x1f;
            x -= 0x10;
            y += 0x10;
            z -= 0x10;
            v->x = x;
            v->y = y;
            v->z = z;
            index = (index + 1) & mask;
            gRandomIndex = index;
            threshold = ((AdvanceAndSample(&table, index) >> 4) & 0x7f) + 0x20;
            sub_08127590(position, &velocity, 0x83A6, 0x99CB, *statePtr, threshold, kind, zero,
                         PaletteIndex(*statePtr, *parameterPtr), zero, i & 1, zero, zero, zero,
                         zero);
            i++;
        } while (i <= 3);
    }
    counter = object->counter;
    index = (gRandomIndex + 1) & 0x3ff;
    gRandomIndex = index;
    if (counter >= ((s32)(((Sample(0x0203B400, index) >> 4) & 0x7f) + 0x20)))
        Task_Kill(object);
    else
        object->counter++;
}

extern struct SceneScriptCallbacks gUnk_0824F7E0;

void SceneScriptTask_Update(struct SceneScriptObject *task)
{
    struct SceneScriptCallbacks callbacks = gUnk_0824F7E0;
    if (task->finished <= 1)
        Callback_InvokeActor(task, callbacks.entries[task->finished]);
}

void Shadow_Destroy(void *);

void DungeonProp_Destroy(struct SceneScriptObject *object)
{
    Sprite_Remove(&object->sprite);
    Collider_Remove(&object->collider);
    if (object->state == 0xc)
        Shadow_Destroy(object->shadow);
}

void SceneScriptObject_ApplyHitFlash(struct ContactCollider *source, void *unused,
                                     struct SceneScriptObject *object)
{
    u8 scratch[4]; /* unused; the original reserves a 4-byte frame */
    struct {
        struct SceneScriptObject *object;
    } context;

    context.object = object;
    if (!(source->reactionFlags & 0x40))
        --context.object->lifetime;
    if (context.object->lifetime > 0) {
        context.object->fadeTimer = 6;
        context.object->holdTimer = 15;
        context.object->resource.paletteIndex = 0x192;
        CpuSet(gPaletteBankData + 0x3240, context.object->resource.palette, 0x04000008);
        Sound_PlaySong(0x13E);
    }
}

void SceneScriptObject_InitHitCollider(struct SceneScriptObject *object)
{
    void *collider = &object->collider;
    struct ContactPackedVector bounds, offset;
    bounds.x = 96;
    bounds.y = 128;
    bounds.z = 96;
    offset.x = 0;
    offset.y = 64;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5001, 0, 16, &bounds, &offset);
    Collider_SetPosition(collider, &object->sprite.position, 0);
    Collider_SetCallback(collider, SceneScriptObject_ApplyHitFlash, object);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    Collider_Refresh(collider);
}

void Shadow_Initialize(void *, void *, u32);

s32 SceneScriptObject_Initialize(struct SceneScriptObject *object, u32 caller)
{
    struct ContactSwarmSprite *sprite = &object->sprite;
    struct DisplaySequenceState *sequence;
    object->caller = caller;
    if (Script_SeekToKeyword('r'))
        object->reveal = Script_GetValue();
    if (Script_SeekToKeyword('p')) {
        sprite->position.x = Script_GetValue();
        sprite->position.y = Script_GetValue();
        sprite->position.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('l'))
        object->lifetime = Script_GetValue();
    else
        object->lifetime = 2;
    if (Script_SeekToKeyword('b'))
        object->pendingScript = Script_GetValue();
    else
        object->pendingScript = 0;
    if (Script_SeekToKeyword('t'))
        object->state = Script_GetValue();
    else
        object->state = 0;
    if (Script_SeekToKeyword('P'))
        object->parameter = Script_GetValue();
    else
        object->parameter = -1;
    {
        s32 ok;
        if (object->state == 14)
            ok = AnimData_Load(&object->resource, 0x6BEA);
        else
            ok = AnimData_Load(&object->resource, 0xB07C);
        if (!ok)
            return -1;
    }
    Sprite_Init(sprite, &object->resource, 0);
    object->proc = ScriptProc_Start(0x922E, 0xB5F1);
    sprite->priority = 2;
    if (object->state == 14) {
        sprite->frame = 0;
        if (object->reveal)
            sprite->flags |= 4;
    } else {
        sequence = &object->resource.frames;
        Anim_SetSequence(sequence, object->proc, (u16)object->state, 0, (u8)object->reveal);
        PlayerCue_StepSequence((void *)sprite, sequence);
    }
    object->resource.paletteIndex = PaletteIndex(object->state, object->parameter);
    object->resource.paletteData = object->resource.palette;
    {
        u32 mode = 0x04000008;
        s32 bank = PaletteIndex(object->state, object->parameter);
        object->resource.paletteIndex = bank;
        CpuSet(gPaletteBankData + bank * 32, object->resource.palette, mode);
    }
    SceneScriptObject_InitHitCollider(object);
    if (object->state == 12) {
        sprite->position.y += 0xC0;
        Shadow_Initialize(object->shadow, &sprite->position, 1);
        ActorShadow_Enable(object->shadow);
    }
    object->fadeTimer = 0;
    object->holdTimer = 0;
    object->finished = 0;
    object->counter = 0;
}
