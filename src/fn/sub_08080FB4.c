// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "script_utility.h"
#define SceneActorReaction_BeginHiddenDefeat sub_08080FB4
void sub_0807C72C(void *, u32, u32, u32);
void sub_0807E7B0(void *, u32);
void *sub_081D4F20(u32, u32);
void sub_081D5204(void *);
void sub_0806FA74(u16, u16);
extern u16 gUnk_030039F0;
static inline u8 IsPrimary(struct SceneActorRegistryResetView *actor)
{
    if (actor->object == (void *)1)
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    /* The actor stores flags2D4 as a halfword and flags2D0 as a word. */
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor)
{
    void *callback = (void *)sub_0807FC90;
    u8 kind = 16;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    {
        /* Preserve the short-lived halfword-clear address. */
        register u32 off asm("r2") = 0x2C8;
        {
            u16 *field = (void *)((u8 *)actor + off);
            *field = 0;
        }
    }
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void SceneActorReaction_BeginHiddenDefeat(struct SceneActorRegistryResetView *actor)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (!SceneActorReaction_TakePrepared(actor))
        return;
    if (IsPrimary(actor)) {
        {
            struct SceneActorRegistryControl *control = &actor->group.control;
            u32 fieldOffset = 0x2CC;
            u32 *field;
            u32 mask;
            u32 zero;
            field = (void *)((u8 *)actor + fieldOffset);
            mask = ~0x141C0;
            *field &= mask;
            zero = 0;
            control->counter = zero;
            *(u16 *)((u8 *)control + 0x14) = zero;
            control->timer = zero;
        }
        /* Frame127 releases attachments without consuming offset outputs. */
        sub_0807C72C(actor, 0, 127, 0);
        sub_0807C72C(actor, 1, 127, 0);
        sub_0807C72C(actor, 3, 127, 0);
        sub_0807E7B0(actor, 4);
    }
    {
        u32 showFlag = 1;
        if (actor->drawableKind == 0) {
            u32 *sequence = actor->drawable;
            *sequence |= showFlag;
        } else {
            u8 *tile = (u8 *)actor->drawable + 0x20;
            *(u32 *)(tile + 8) |= showFlag;
        }
    }
    actor->field2B0 = 1;
    script = *(u16 *)((u8 *)actor + 0x1F8);
    if (script) {
        register u32 offset asm("r4") = 0;
        values[0] = *(s16 *)((u8 *)&actor->position + offset);
        values[1] = actor->position.y;
        values[2] = actor->position.z;
        offset = 0x1FC;
        values[3] = *(s32 *)((u8 *)actor + offset);
        values[4] = *(s32 *)((u8 *)actor + 0x200);
        values[5] = *(s32 *)((u8 *)actor + 0x204);
        offset += 0xE4;
        values[6] = *(s32 *)((u8 *)actor + offset);
        values[7] = actor->id;
        arguments.count = 8;
        arguments.values = values;
        /* Keep the payload offset live through argument construction. */
        asm("" : : "r"(offset));
        Script_Call(script, &arguments);
    }
    if (!Flag(actor, 0x4000, 0x2D4) && (u32)(actor->category - 11) > 2)
        sub_081D5204(sub_081D4F20(actor->category, actor->paletteOffset));
    actor->fieldAA = 0;
    SetControl(actor);
    SetAnimation(actor, 1, 0, 2);
    if (!Test(gUnk_030053F4, 0x4800) && !Flag(actor, 0x800, 0x2D0))
        sub_0806FA74(gUnk_030039F0, actor->id);
    ActorShadow_Disable((u8 *)actor + 0x320);
    switch (actor->category) {
        case 4:
            Sound_PlaySong(212);
            break;
        case 1:
            Sound_PlaySong(253);
            break;
        case 3:
            sub_08077848(actor, 1);
            break;
        case 25:
            Sound_PlaySong(459);
            break;
        case 10:
            Sound_PlaySong(318);
            break;
        case 21:
            Sound_PlaySong(318);
            break;
    }
}
