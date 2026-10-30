#include "global.h"
#define Ivy_UpdateDefeatBurst sub_080F421C
#include "engine.h"
#include "script_utility.h"
#include "scene_vec.h"
#include "contact_collider.h"
struct IvyBurstVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct IvyBurstScene {
    u8 unknown0[0x1C];
    u32 mode;
};
struct SwordPhase {
    u8 flags;
    u8 unknown1[4];
    u8 state, phase;
} __attribute__((packed));
struct SwordMotion {
    u8 unknown0[0x29];
    struct SwordPhase phase;
    u8 unknown30[0x44];
    void *update;
    u8 unknown78[0x2C];
};
struct SwordComponent {
    u8 unknown0[0xF0];
    struct SwordMotion motion;
};
struct IvyBurstPart {
    u32 flags;
    u8 unknown4[0x28];
};
struct IvyBurstPrefix {
    u8 unknown0[0x60];
    struct IvyBurstScene *scene;
    u8 unknown64[0x22];
    s16 lifeParameter;
    u8 unknown88[5];
    struct SwordPhase control;
    u8 unknown94[0xC];
    u8 flash, bursts, started, unknownA3;
    u16 lifeValue;
    u8 unknownA6[2];
    struct IvyBurstPart parts[4];
    u8 unknown158[0x70];
    struct ContactCollider attack[4];
    struct ContactCollider body[4];
    u8 unknown468[0x54];
    struct SwordComponent children[4];
    u8 unknownB0C[0x5C];
    u32 effectFlags;
    u8 unknownB6C[2];
    u8 direction;
    u8 unknownB6F[0x15];
    struct IvyBurstVector position;
    u8 unknownB8C[0x24];
    u8 shadow[0x6C];
    struct ContactCollider colliderA, colliderB;
    u8 unknownCC4[0x2C];
    void *callback;
    u8 unknownCF4[0x3A];
    u16 counter;
    u8 unknownD30[0x24];
    u8 done, bestiaryIndex;
    u8 unknownD56[9];
    u8 burstIndex;
    void *lifeState;
    u8 unknownD64[8];
    u32 script;
    s32 scriptArgument0, scriptArgument1;
};
extern const s16 gUnk_086149C4[];
void TargetMotion_UpdateFlashingExit(void);
void sub_08159164(u32);
void Player_RefreshLifeState(void *, s32);
void ScaledShadow_Disable(void *);
s32 SparkParticle_Spawn(const void *, u8, const void *, s32, s32, s32, s32, u32);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
s32 Mod(s32, s32);
static inline void Ivy_ClearStart(struct SwordPhase *control, u32 bits)
{
    u32 mask = ~bits;
    control->flags = mask & control->flags;
}
static inline void Ivy_SetFlags(struct SwordPhase *control, u8 bits)
{
    control->flags = bits | control->flags;
}
static inline u8 Ivy_TakeBurstStart(struct IvyBurstPrefix *actor)
{
    u8 mask = 1;
    if (mask & actor->control.flags) {
        Ivy_ClearStart(&actor->control, 1);
        actor->started = 0;
        return 1;
    }
    return 0;
}
static inline s32 Ivy_ScaleOffset(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void Sword_SetPhase(struct SwordPhase *phase, u8 active, u8 state)
{
    phase->state = active;
    phase->phase = state;
    phase->flags |= active;
}
static inline u8 Sword_ChildIsActive(struct SwordComponent *child)
{
    return child->motion.phase.state;
}
static inline struct SwordPhase *Sword_ChildPhase(struct IvyBurstPrefix *actor, s32 index)
{
    return &actor->children[index].motion.phase;
}
static inline struct SwordMotion *Sword_ChildMotion(struct IvyBurstPrefix *actor, s32 index)
{
    return &actor->children[index].motion;
}
void Ivy_UpdateDefeatBurst(struct IvyBurstPrefix *actor)
{
    s32 remainder;
    if (Ivy_TakeBurstStart(actor)) {
        s32 i = 0;
        struct IvyBurstPart *parts = actor->parts;
        for (; i <= 3; i++) {
            if (Sword_ChildIsActive(&actor->children[i])) {
                struct SwordMotion *motion = Sword_ChildMotion(actor, i);
                Sword_SetPhase(Sword_ChildPhase(actor, i), 1, 5);
                motion->update = TargetMotion_UpdateFlashingExit;
            }
        }

        for (i = 0; i <= 3; i++) {
            u32 *flags;
            flags = &parts[i].flags;
            if (!((*flags) & 1)) {
                *flags |= 1;
                actor->attack[i].flags |= 4;
                actor->body[i].flags |= 4;
            }
        }

        actor->colliderA.flags |= 4;
        actor->colliderB.flags |= 4;
        actor->counter = 0;
        actor->burstIndex = 0;
        actor->flash = 0;
        actor->bursts = 30;
        sub_08159164(actor->lifeValue);
        Player_RefreshLifeState(actor->lifeState, actor->lifeParameter);
        actor->done = 1;
    }
    if (actor->callback)
        Callback_InvokeActor(actor, actor->callback);
    remainder = Mod(actor->counter, 2);
    if (remainder == 0) {
        struct IvyBurstVector range;
        struct IvyBurstVector position;
        struct SceneVec offset;
        actor->effectFlags |= 1;
        actor->flash = 0;
        actor->bursts--;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        position = actor->position;
        ZeroFill8(&offset);
        switch (actor->burstIndex) {
            case 0:
                offset.z = (offset.x = -120);
                actor->burstIndex++;
                Sound_PlaySong(366);
                break;

            case 1:
                offset.z = (offset.x = -75);
                actor->burstIndex++;
                break;

            case 2:
                offset.z = (offset.x = -30);
                actor->burstIndex++;
                break;

            case 3:
                offset.z = (offset.x = 15);
                actor->burstIndex++;
                break;

            case 4:
                offset.z = (offset.x = 60);
                actor->burstIndex = 0;
                break;
        }

        position.x = (Ivy_ScaleOffset(offset.x * gUnk_086149C4[(actor->direction + 64) & 255]) +
                      Ivy_ScaleOffset(offset.z * gUnk_086149C4[actor->direction])) +
                     position.x;
        position.z = (Ivy_ScaleOffset(-(offset.x * gUnk_086149C4[actor->direction])) +
                      Ivy_ScaleOffset(offset.z * gUnk_086149C4[(actor->direction + 64) & 255])) +
                     position.z;
        SparkParticle_Spawn(&position, 2, &range, 0, 0, 22, 2, 1);
        if (!actor->bursts) {
            ScaledShadow_Disable(actor->shadow);
            actor->control.state = 0;
            actor->control.phase = 0;
            Ivy_SetFlags(&actor->control, 1);
            if ((actor->scene->mode != 4) && (actor->script)) {
                s32 values[8];
                struct ScriptUtilityArguments arguments;
                values[0] = actor->position.x;
                values[1] = actor->position.y;
                values[2] = actor->position.z;
                values[3] = actor->scriptArgument0;
                values[4] = actor->scriptArgument1;
                values[5] = -1;
                values[6] = -1;
                values[7] = -1;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(actor->script, &arguments);
                GameState_SetEventFlag(BestiaryDisplay_GetEntryForActor(11, actor->bestiaryIndex));
            }
        }
    } else {
        actor->effectFlags &= ~1;
        actor->flash++;
    }
    (actor->counter)++;
}
