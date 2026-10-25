#include "global.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: disables contact and sequences bursts before recording a defeat event.
#define Ivy_UpdateDefeatBurst sub_080F421C
#ifdef NONMATCHING
// fuzzy: 72.4%
// fix.py best (sweep after the inlined-insn fix)
// Complete defeat flow; child-loop induction and local allocation still differ.
#include "engine.h"
#include "script_utility.h"
#include "scene_vec.h"
struct IvyBurstVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct IvyBurstScene {
    u8 unknown0[0x1C];
    u32 mode;
};
struct IvyBurstPrefix {
    u8 unknown0[0x60];
    struct IvyBurstScene *scene;
    u8 unknown64[0x22];
    s16 lifeParameter;
    u8 unknown88[5];
    u8 flags;
    u8 unknown8E[4];
    u8 state, phase;
    u8 unknown94[0xC];
    u8 flash, bursts, started, unknownA3;
    u16 lifeValue;
};
struct IvyBurstChild {
    u8 unknown0[0x29];
    u8 flags, unknown2A[4], state, phase;
    u8 unknown30[0x44];
    void *update;
    u8 unknown78[0x11C];
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
struct IvyBurstPart {
    u32 flags;
    u8 unknown4[0x28];
};
static inline void Ivy_ClearStart(u8 *field, s32 mask)
{
    *field &= mask;
}
static inline u8 Ivy_TakeBurstStart(struct IvyBurstPrefix *actor)
{
    u32 mask = 1;
    u32 flags = actor->flags;
    if (flags & mask) {
        Ivy_ClearStart(&actor->flags, ~1);
        actor->started = 0;
        return 1;
    }
    return 0;
}
static inline s32 Ivy_ScaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void Ivy_UpdateDefeatBurst(struct IvyBurstPrefix *actor)
{
    s32 remainder;
    if (Ivy_TakeBurstStart(actor)) {
        s32 i;
        struct IvyBurstPart *parts = (void *)(((u8 *)actor) + 0xA8);
        u8 *head = ((u8 *)actor) + 0x5D5;
        u8 *child = ((u8 *)actor) + 0x5AC;
        for (i = 0; i <= 3; i++, head += 0x194, child += 0x194) {
            if (((u8 *)actor)[0x5DA + (i * 0x194)]) {
                head[5] = 1;
                head[6] = 5;
                {
                    u8 k1 = 1;
                    head[0] |= k1;
                }
                *((void **)(child + 0x74)) = TargetMotion_UpdateFlashingExit;
            }
        }

        for (i = 0; i <= 3; i++) {
            u32 *flags;
            asm("" : "+r"(i));
            flags = &parts[i].flags;
            if (!((*flags) & 1)) {
                *flags |= 1;
                *((u16 *)((((u8 *)actor) + 0x1CE) + (i * 0x54))) |= 4;
                *((u16 *)((((u8 *)actor) + 0x31E) + (i * 0x54))) |= 4;
            }
        }

        *((u16 *)(((u8 *)actor) + 0xC22)) |= 4;
        *((u16 *)(((u8 *)actor) + 0xC76)) |= 4;
        *((u16 *)(((u8 *)actor) + 0xD2E)) = 0;
        ((u8 *)actor)[0xD5F] = 0;
        actor->flash = 0;
        actor->bursts = 30;
        sub_08159164(actor->lifeValue);
        Player_RefreshLifeState(*((void **)(((u8 *)actor) + 0xD60)), actor->lifeParameter);
        ((u8 *)actor)[0xD54] = 1;
    }
    if (*((void **)(((u8 *)actor) + 0xCF0)))
        Callback_InvokeActor(actor, *((void **)(((u8 *)actor) + 0xCF0)));
    remainder = Mod(*((u16 *)(((u8 *)actor) + 0xD2E)), 2);
    if (remainder == 0) {
        struct IvyBurstVector range;
        struct IvyBurstVector position;
        struct SceneVec offset;
        u32 zero;
        *((u32 *)(((u8 *)actor) + 0xB68)) |= 1;
        actor->flash = 0;
        actor->bursts--;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        position = *((struct IvyBurstVector *)(((u8 *)actor) + 0xB84));
        zero = remainder;
        CpuSet(&zero, &offset, 0x05000002);
        switch (((u8 *)actor)[0xD5F]) {
            case 0:
                offset.z = (offset.x = -120);
                ((u8 *)actor)[0xD5F]++;
                Sound_PlaySong(366);
                break;

            case 1:
                offset.z = (offset.x = -75);
                ((u8 *)actor)[0xD5F]++;
                break;

            case 2:
                offset.z = (offset.x = -30);
                ((u8 *)actor)[0xD5F]++;
                break;

            case 3:
                offset.z = (offset.x = 15);
                ((u8 *)actor)[0xD5F]++;
                break;

            case 4:
                offset.z = (offset.x = 60);
                ((u8 *)actor)[0xD5F] = 0;
                break;
        }

        position.x = (Ivy_ScaleOffset(offset.x * gUnk_086149C4[(((u8 *)actor)[0xB6E] + 64) & 255]) +
                      Ivy_ScaleOffset(offset.z * gUnk_086149C4[((u8 *)actor)[0xB6E]])) +
                     position.x;
        position.z =
            (Ivy_ScaleOffset(-(offset.x * gUnk_086149C4[((u8 *)actor)[0xB6E]])) +
             Ivy_ScaleOffset(offset.z * gUnk_086149C4[(((u8 *)actor)[0xB6E] + 64) & 255])) +
            position.z;
        SparkParticle_Spawn(&position, 2, &range, 0, 0, 22, 2, 1);
        if (!actor->bursts) {
            ScaledShadow_Disable(((u8 *)actor) + 0xBB0);
            actor->state = 0;
            actor->phase = 0;
            actor->flags |= 1;
            if ((actor->scene->mode != 4) && (*((u32 *)(((u8 *)actor) + 0xD6C)))) {
                s32 values[8];
                struct ScriptUtilityArguments arguments;
                values[0] = *((s16 *)(((u8 *)actor) + 0xB84));
                values[1] = *((s16 *)(((u8 *)actor) + 0xB86));
                values[2] = *((s16 *)(((u8 *)actor) + 0xB88));
                values[3] = *((s32 *)(((u8 *)actor) + 0xD70));
                values[4] = *((s32 *)(((u8 *)actor) + 0xD74));
                values[5] = -1;
                values[6] = -1;
                values[7] = -1;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(*((u32 *)(((u8 *)actor) + 0xD6C)), &arguments);
                GameState_SetEventFlag(BestiaryDisplay_GetEntryForActor(11, ((u8 *)actor)[0xD55]));
            }
        }
    } else {
        *((u32 *)(((u8 *)actor) + 0xB68)) &= ~1;
        actor->flash++;
    }
    (*((u16 *)(((u8 *)actor) + 0xD2E)))++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080F421C);
#endif
