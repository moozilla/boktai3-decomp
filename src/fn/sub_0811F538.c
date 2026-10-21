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
s32 ActorAbility_ActivatePaletteEffect(struct ActorAbility *ability)
{
    u8 *owner, *stats;
    u32 zero;
    s32 cost;
    ability->request = 255;
    if (Modifier_IsFourthSourceReady(ability)) {
        owner = ability->owner;
        stats = owner + 0x18;
        zero = ability->cooldown;
        if (zero == 0) {
            cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
            if (*(s16 *)(owner + 0x70) >= cost) {
                s32 palette;
                ability->cooldown = ability->interval;
                CallbackList_SubtractStateTenValue(stats, ability, cost);
                palette = CharacterModel_GetPrimaryPaletteIndex(owner + 0x610);
                PalFade_Start(stats, 0x385, 0x385, palette, *(u16 *)(owner + 0x96), 16, 4, zero);
                ability->remaining = ability->duration;
                Sound_PlaySong(0x3e3);
            }
        }
    }
    return 0;
}
