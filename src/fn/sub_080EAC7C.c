#include "global.h"

#ifdef NONMATCHING
// fuzzy: 85.0%
// Remaining differences: register allocation and instruction scheduling; see notes/codex-low.md.
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "gba/macro.h"
#include "gba/syscall.h"

struct Player;
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Player_ApplyPositionRequest(struct Player *, struct SceneVec *, s32, u32, s32, s32, s32, u32);
s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);
void sub_0807D2E0(void *, u32, struct ContactPosition *, u32, u32, u32);
void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);
void DamageNumberDisplay_Show(void *, u32, s32, void *, void *, u32);
u32 Octopus_GetAnimationStatus(struct SceneActorRegistryResetView *);
void Octopus_UpdateActiveState(void);
extern u8 *gUnk_02000580;
extern s16 *gUnk_020004B4;

struct OctopusContext {
    u8 unknown0[0x628];
    u32 flags;
    u8 unknown630[0xC];
    u8 startTick, holdTicks, interval;
};

struct OctopusTarget {
    u8 unknown0[8];
    u8 kind;
    u8 unknown9[0x33];
    void *player;
};

struct OctopusDrawable {
    u8 unknown0[7];
    u8 mode : 8;
};

struct OctopusOffset {
    u16 x, y, z, pad;
};
extern const struct OctopusOffset gUnk_086062B0[2][5];

static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

struct GolemRoutePoint {
    s16 x, z;
};
struct GolemRouteLink {
    u8 unknown0[8];
    struct GolemRoutePoint *point;
};

void SceneActorBehavior_PlayReactionSound(void *);
void Camera_RequestShake(u32);
void SpriteParticle_SpawnBatch(void *, u32, u32, u32, u32);
void SceneActorReaction_RestoreAlternateAfterDelay(void);

void SceneActor_EndReactionAnimation(void);
#define Octopus_UpdateStagger sub_080EAC7C
void Octopus_UpdateStagger(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->drawable + 0x48);
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 3);
        actor->field2B3 = 0;
        {
            u16 *d = &actor->colliderA.flags;
            u32 flags = *d;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *d = mask;
        }
        {
            u32 bit = 0x800;
            actor->flags2C0 = bit | actor->flags2C0;
        }
        {
            u32 bit = 0x20;
            actor->flags2D4 = bit | actor->flags2D4;
        }
    }
    {
        u32 bit = 0x10;
        actor->flags2D6 = bit | actor->flags2D6;
    }
    if (frame <= 17 && sequence->index == 0) {
        u16 elapsed = sequence->elapsed;
        u8 duration = sequence->duration;
        if (elapsed == (duration != 0) && duration != 1)
            sequence->elapsed = 0;
    }
    if (actor->field2B3) {
        actor->field2B0 = 0;
        actor->flags2D4 &= ~0x20;
        actor->flags2CC &= ~0x80;
        SetCtl(actor, SceneActor_EndReactionAnimation, 0x23);
        SetState(actor, 1, 0, 3);
        actor->direction = actor->fieldE9;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080EAC7C);
#endif
