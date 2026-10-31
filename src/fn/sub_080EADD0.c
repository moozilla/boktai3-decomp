#include "scene_actor_reaction.h"
#include "scene_actor_chase_reaction.h"
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

static inline u16 Octopus_RandomSample(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)table + index);
}

static inline u16 Octopus_RandomSpeed(void)
{
    return (Octopus_RandomSample(gRandomTable) & 7) + 0x48;
}

static inline u32 Octopus_SpinSpeed(u32 value)
{
    return value | ~0x1F;
}

inline static u32 Octopus_MotionBits(u32 mask, u16 *flags)
{
    return (*flags) & mask;
}

struct OctopusDefeatMessage {
    u16 kind, value;
};
static inline void Octopus_MessageValue(void *object, u16 value)
{
    struct OctopusDefeatMessage *message = object;
    message->value = value;
}

static inline void Octopus_EnableDefeatMotion(struct SceneActorRegistryResetView *actor, u16 bit,
                                              u16 **component)
{
    u16 *field = &actor->flags2D4;
    *component = field;
    *field = bit | *field;
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
            collider->flags |= 4;
            collider->flags &= 0xFFF7;
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
            actor->health = k1;
        }
        actor->field2EA = 0;
        Memory_Clear(context + 0x624, 8);
        {
            s32 abs;
            actor->secondaryMotion.y = Octopus_RandomSpeed();
            *((s16 *)(((u8 *)context) + 0x62A)) =
                Octopus_SpinSpeed(Octopus_RandomSample(gRandomTable));
            actor->field2E8 = 0;
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
            u16 *flags;
            struct ContactPosition *pos;
            Octopus_EnableDefeatMotion(actor, 0x40, &flags);
            actor->health = 0;
            pos = &actor->body.position;
            SpriteParticle_SpawnBatch(pos, 1, 2, 0, 0x154);
            Sound_PlaySong(0x1D8);
            {
                u8 *d;
                u32 bit = 1;
                if (actor->drawableKind == 0) {
                    u32 *sequence = actor->drawable;
                    *sequence |= bit;
                    d = (void *)sequence;
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
                    u32 mask;
                    d += 0x20;
                    mask = ~2;
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
            if ((!Octopus_MotionBits(0x4000, flags)) && (((u32)(actor->category - 11)) > 2))
                GameState_SetEventFlag(
                    BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
            SceneActorChase_SetTypedControl(actor, (void *)SceneActorControl_OnIdle, 0x10);
            SceneActorChase_SetTypedState(actor, 1, 0, 2);
            {
                u8 *linked = actor->context;
                struct OctopusDefeatMessage *message = (void *)values;
                u32 id;
                message->kind = 8;
                Octopus_MessageValue(message, 0);
                id = *((u32 *)(((u8 *)linked) + 0x630));
                if (id)
                    SceneActorRegistry_SendToActorId(15, values, id);
            }
        } else if (Mod(frame, 4) == 0)
            actor->secondaryMotion.y -= 8;
    }
}
