#define ActorAbility_IsReady Modifier_IsFourthSourceReady
// Tentative descriptive names; inferred from static field accesses and callees.
#include "global.h"
#include "actor_stats.h"
struct ActorAbility {
    u8 unknown0[6];
    u16 interval, cost;
    u8 unknownA[2];
    u16 duration;
    u8 unknownE[2];
    u8 enabled, request;
    u16 cooldown, remaining;
    u8 soundTimer, started;
    u8 *owner;
};
u8 Modifier_IsFourthSourceReady(struct ActorAbility *);
s32 CallbackList_RunMatchingState(void *, u32, s32);
void CallbackList_SubtractStateTenValue(void *, struct ActorAbility *, s32);
s32 CharacterModel_GetPrimaryPaletteIndex(u8 *);
void PalFade_Start(void *, u32, u32, s32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
void Character_SetLowResourceColorEffect(void *, void *);
s32 ActorAbility_UpdateRecovery(struct ActorAbility *ability)
{
    u8 *owner;
    struct ActorStats *stats;
    s32 cost, palette;
    u32 started;
    ability->request = 255;
    if (!Modifier_IsFourthSourceReady(ability))
        return 0;
    owner = ability->owner;
    stats = (struct ActorStats *)(owner + 0x18);
    if (stats->amount >= stats->amountLimit)
        return 0;
    if (!ability->started && *(s16 *)(owner + 0x70) <= 15)
        return 0;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (stats->value < cost)
        return 0;
    CallbackList_SubtractStateTenValue(stats, ability, cost);
    stats->amount++;
    if (stats->amount > stats->amountLimit)
        stats->amount = stats->amountLimit;
    palette = CharacterModel_GetPrimaryPaletteIndex(owner + 0x610);
    started = ability->started;
    if (started == 0) {
        PalFade_Start(stats, 0x386, 0x386, palette, *(u16 *)((u8 *)stats + 0x7e), 15, 4, started);
        ability->started = 1;
    } else if (!(*(u32 *)(owner + 0x8e4) & 1)) {
        PalFade_Start(stats, 0x386, 0x386, palette, *(u16 *)((u8 *)stats + 0x7e), 15, 4, 1);
    }
    if (ability->soundTimer == 0)
        ability->soundTimer = 30;
    else {
        ability->soundTimer--;
        if (ability->soundTimer == 10)
            Sound_PlaySong(0x95);
    }
    Character_SetLowResourceColorEffect(stats->state44, stats->state48);
    return 1;
}
