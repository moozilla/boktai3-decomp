#define ActorAbility_IsReady Modifier_IsFourthSourceReady
// Tentative descriptive names; inferred from static field accesses and callees.
#include "global.h"
struct ActorAbility {
    u8 unknown0[6];
    u16 interval, cost;
    u8 unknownA[2];
    u16 duration;
    u8 unknownE[2];
    u8 enabled, request;
    u16 cooldown, remaining;
    u8 unknown16[2];
    u8 *owner;
};
u8 Modifier_IsFourthSourceReady(struct ActorAbility *);
s32 CallbackList_RunMatchingState(void *, u32, s32);
void CallbackList_SubtractStateTenValue(void *, struct ActorAbility *, s32);
s32 CharacterModel_GetPrimaryPaletteIndex(u8 *);
void PalFade_Start(void *, u32, u32, s32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
struct ProjectileParameters {
    u32 a, b, c, d;
};
s32 SceneProcSprites_CreateEntry(void *, s32, u32, s32, void *);
s32 ActorAbility_SpawnProjectile(struct ActorAbility *ability)
{
    u8 *owner, *stats;
    u32 offset;
    s32 height, cost;
    /* Unused vector retained: agbcc emits these three stores in the original. */
    struct ProjectileParameters args;
    ability->request = 255;
    if (!Modifier_IsFourthSourceReady(ability))
        goto fail;
    owner = ability->owner;
    stats = owner + 0x18;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (*(s16 *)(owner + 0x70) < cost)
        goto fail;
    offset = 0x8e8;
    args.a = *(u32 *)(owner + offset);
    args.b = 0;
    args.c = 0;
    height = *(s32 *)(stats + 0x38) - 0x12000;
    if (SceneProcSprites_CreateEntry(owner + 0x28, height, *(u32 *)(owner + offset),
                                     (s8) * (u8 *)(owner + (offset + 0x10)),
                                     (u8 *)ability + 4) < 0) {
    fail:
        return 0;
    }
    CallbackList_SubtractStateTenValue(stats, ability, cost);
    ability->remaining = ability->duration;
    return 1;
}
