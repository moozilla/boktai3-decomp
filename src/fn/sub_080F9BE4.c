#include "global.h"

#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *);
void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);
void Axe_ReleaseAuxiliarySprite(void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
s32 Mod(s32, s32);
void CpuSet(const void *, void *, u32);
void Sound_PlaySong(u32);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -(-x >> 12);
    return result;
}
static inline void SetAnimation(struct SceneActorRegistryResetView *a, u8 s, u8 p, u8 b)
{
    a->state2A8 = s;
    a->phase2A9 = p;
    a->behavior2AA = b;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
inline static void CpuSet_2(int a2, struct ContactPosition *a1, u32 *a0)
{
    CpuSet(a0, a1, a2);
}

void Axe_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct {
        u32 zero;
        s32 values[8];
    } buf;
    struct ContactPackedVector range;
    struct ContactPosition offset;
    union {
        struct ContactPosition p;
        struct ContactPackedVector v;
    } position;
    struct ScriptUtilityArguments arguments;
    u8 *context = actor->context;
    u8 *drawable = actor->drawable;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectProfileEntry(actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            struct ContactCollider *c = &actor->colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &actor->colliderA;
            c->flags |= 4;
        }
        {
            u32 mask = 0x800;
            actor->flags2C0 = mask | actor->flags2C0;
        }
        context[0xED2] = 0;
        Axe_ReleaseAuxiliarySprite(actor);
    }
    if (Mod(frame, 2) == 0) {
        s32 x, z, x2;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        position.p = actor->body.position;
        {
            struct ContactPosition *op = &offset;
            buf.zero = 0;
            CpuSet_2(0x05000002, op, &buf.zero);
        }
        switch (context[0xED2]) {
            case 0:
                offset.x = -120;
                offset.z = -120;
                context[0xED2]++;
                Sound_PlaySong(0x16E);
                break;
            case 1:
                offset.x = -75;
                offset.z = -75;
                context[0xED2]++;
                break;
            case 2:
                offset.x = -30;
                offset.z = -30;
                context[0xED2]++;
                break;
            case 3:
                offset.x = 15;
                offset.z = 15;
                context[0xED2]++;
                break;
            case 4:
                offset.x = 60;
                offset.z = 60;
                context[0xED2] = 0;
                break;
        }
        x = Scale(offset.x * gUnk_086149C4[(drawable[6] + 64) & 255]);
        z = Scale(offset.z * gUnk_086149C4[drawable[6]]);
        position.v.x = x + z + position.p.x;
        x2 = Scale(-(offset.x * gUnk_086149C4[drawable[6]]));
        z = Scale(offset.z * gUnk_086149C4[(drawable[6] + 64) & 255]);
        position.v.z = x2 + z + position.p.z;
        SparkParticle_Spawn(&position, 2, &range, 0, 0, 22, 2, 1);
    }
    if (!Sprite_BlinkUntilExpired(actor->drawable, frame, 10, 30))
        return;
    actor->field2B0 = 1;
    SceneActorBehavior_CallTerrainScript((void *)actor);
    script = *(u16 *)((u8 *)actor + 0x1F8);
    if (script) {
        buf.values[0] = actor->body.position.x;
        buf.values[1] = actor->body.position.y;
        buf.values[2] = actor->body.position.z;
        buf.values[3] = *(s32 *)((u8 *)actor + 0x1FC);
        buf.values[4] = *(s32 *)((u8 *)actor + 0x200);
        buf.values[5] = *(s32 *)((u8 *)actor + 0x204);
        buf.values[6] = actor->field2E0;
        buf.values[7] = actor->body.id;
        arguments.count = 8;
        arguments.values = buf.values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        if (!(actor->flags2D4 & mask) && (u32)(actor->category - 11) > 2)
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    }
    GameState_SetEventFlag(BestiaryDisplay_GetEntryForActor(actor->category, context[0xED7]));
    actor->fieldAA = 0;
    {
        void *callback = (void *)SceneActorControl_OnIdle;
        u32 kind;
        kind = 16;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
    }
    SetAnimation(actor, 1, 0, 2);
}
