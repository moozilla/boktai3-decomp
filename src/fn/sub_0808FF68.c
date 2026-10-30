#include "scene_actor_control_phase.h"
/* SceneActorControl_CompleteScriptedResponse_0809F368: SceneActorControl_CompleteScriptedResponse with other constants (ported by relatives.py, fixed by hand; provisional name) */
#include "scene_actor_reaction.h"
#include "script_utility.h"
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *);
void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

extern const s16 gUnk_086149C4[];
void Slime_EmitDeathDebris(void *);
static inline s32 Shift(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Shift(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Shift(gUnk_086149C4[angle] * speed);
}
// confidence=low: invokes a pending Slime script with motion and variant arguments, then resets control.
void Slime_RunPendingActionScript(struct SceneActorRegistryResetView *actor)
{
    s32 values[10];
    struct ContactPosition vector;
    struct ScriptUtilityArguments args;
    u8 *body = actor->context;
    if (*(u16 *)(body + 0x404)) {
        values[0] = actor->body.position.x;
        values[1] = actor->body.position.y;
        values[2] = actor->body.position.z;
        values[3] = *(s16 *)((u8 *)actor + 0x9C);
        Polar(&vector, actor->field2B6, 12);
        values[4] = vector.x;
        values[5] = 24;
        values[6] = vector.z;
        values[7] = actor->paletteOffset;
        values[8] = *(u16 *)(body + 0x406);
        values[9] = *(u16 *)((u8 *)actor + 0xB0);
        args.count = 10;
        args.values = values;
        Script_Call(*(u16 *)(body + 0x404), &args);
        Slime_EmitDeathDebris(actor);
        *(u16 *)(body + 0x404) = 0;
        {
            u32 mask = 8;
            u16 *p = (void *)(body + 0x40A);
            *p = (*(&mask)) | (*p);
        }
        {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind = 16;
            actor->field2B2 = 1;
            actor->field2B0 = 0;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = 0;
            {
                u32 *p = &actor->flags114;
                u32 mask = ~1;
                *p &= mask;
            }
            actor->word104 = 0;
        }
        SetAnimation(actor, 0, 0, 2);
        {
            u16 *p = &actor->flags2D6;
            u32 mask = ~0x980;
            *p = mask & *p;
        }
    }
}
