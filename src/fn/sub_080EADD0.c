#include "global.h"

#ifdef NONMATCHING
// fuzzy: 85.4%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: defeat state. The body spins (drawable+6) and drifts on the spin vector;
// once the death flag (0x2D0 & 0x10) is set it runs the defeat effects, scripts and bestiary
// flag, returns to idle and notifies the context's linked actor.

#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
#include "ram.h"

void SpriteParticle_SpawnBatch(void *, u32, u32, u32, u32);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorRegistry_SendToActorId(u8, void *, u16);
s32 Mod(s32, s32);
extern const s16 gUnk_086149C4[];

#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))

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

static inline u16 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(index + RANDOM_TABLE_ADDR);
}

inline static u32 Arg1(u32 k_, u16 *flags)
{
    return (*flags) & k_;
}

void Octopus_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u8 *context = actor->context;
    u8 *drawable = actor->drawable;
    s32 values[8];
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 3);
        {
            struct ContactCollider *collider = &actor->colliderA;
            u32 flags = collider->flags;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            collider->flags = mask;
        }
        {
            u32 bit = 0x800;
            actor->flags2C0 = bit | actor->flags2C0;
        }
        {
            u16 bit = 0x40;
            actor->flags2D4 &= ~bit;
        }
        {
            u8 k1 = 1;
            *((u16 *)(((u8 *)actor) + 0x9C)) = k1;
        }
        actor->field2EA = 0;
        Memory_Clear(context + 0x624, 8);
        {
            s32 abs;
            *((u16 *)(((u8 *)actor) + 0x2EE)) = (Rand() & 7) + 0x48;
            *((s16 *)(((u8 *)context) + 0x62A)) = Rand() | (~0x1F);
            *((u16 *)(((u8 *)actor) + 0x2E8)) = 0;
            abs = *((s16 *)(((u8 *)context) + 0x62A));
            if (abs < 0)
                abs = -abs;
            if (abs < 10)
                *((s16 *)(((u8 *)context) + 0x62A)) = 10;
        }
        {
            u32 bit = 2;
            if (actor->drawableKind == 0)
                *((u32 *)actor->drawable) |= bit;
            else {
                u8 *p = ((u8 *)actor->drawable) + 0x20;
                *((u32 *)(p + 8)) |= bit;
            }
        }
        SceneActorPlacement_PlayCategoryEventSound(actor);
    }
    drawable[6] += context[0x62A];
    {
        u32 angle = (0x1A0 - drawable[6]) & 0xFF;
        *((u16 *)(((u8 *)drawable) + 0x20)) -= (gUnk_086149C4[angle & 0xFF] * 33) >> 11;
        *((u16 *)(((u8 *)drawable) + 0x1C)) -= (gUnk_086149C4[(angle + 0x40) & 0xFF] * 33) >> 11;
    }
    {
        u8 k2 = 0x10;
        if (actor->flags2D0 & k2) {
            u16 *flags = &actor->flags2D4;
            struct ContactPosition *pos;
            {
                u32 bit = 0x40;
                *flags = bit | (*flags);
            }
            *((u16 *)(((u8 *)actor) + 0x9C)) = 0;
            pos = &actor->body.position;
            SpriteParticle_SpawnBatch(pos, 1, 2, 0, 0x154);
            Sound_PlaySong(0x1D8);
            {
                u8 *d;
                u32 bit = 1;
                if (actor->drawableKind == 0) {
                    d = actor->drawable;
                    *((u32 *)d) |= bit;
                } else {
                    u8 *p;
                    d = actor->drawable;
                    p = d + 0x20;
                    *((u32 *)(p + 8)) |= bit;
                }
                if (actor->drawableKind == 0) {
                    u32 mask = ~2;
                    *((u32 *)d) &= mask;
                } else {
                    u32 mask = ~2;
                    d += 0x20;
                    *((u32 *)(d + 8)) &= mask;
                }
            }
            SceneActorBehavior_CallTerrainScript(actor);
            {
                u16 script = *((u16 *)(((u8 *)actor) + 0x1F8));
                if (script) {
                    struct ScriptUtilityArguments arguments;
                    values[0] = pos->x;
                    values[1] = actor->body.position.y;
                    values[2] = actor->body.position.z;
                    values[3] = *((u32 *)(((u8 *)actor) + 0x1FC));
                    values[4] = *((u32 *)(((u8 *)actor) + 0x200));
                    values[5] = *((u32 *)(((u8 *)actor) + 0x204));
                    values[6] = actor->field2E0;
                    values[7] = actor->body.id;
                    arguments.count = 8;
                    arguments.values = values;
                    Script_Call(script, &arguments);
                }
            }
            if ((!Arg1(0x4000, flags)) && (((u32)(actor->category - 11)) > 2))
                GameState_SetEventFlag(
                    BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
            SetCtl(actor, (void *)SceneActorControl_OnIdle, 0x10);
            SetState(actor, 1, 0, 2);
            {
                u8 *linked = actor->context;
                u16 *message = (u16 *)values;
                u32 id;
                message[0] = 8;
                message[1] = 0;
                id = *((u32 *)(((u8 *)linked) + 0x630));
                if (id)
                    SceneActorRegistry_SendToActorId(15, values, id);
            }
        } else if (Mod(frame, 4) == 0)
            *((u16 *)(((u8 *)actor) + 0x2EE)) -= 8;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Octopus_UpdateDefeat);
#endif
