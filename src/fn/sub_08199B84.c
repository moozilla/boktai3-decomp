#include "global.h"
#define CallbackSlot_BeginPaletteTransition sub_08199B84
struct CallbackSlot {
    u8 unknown0[8]; u8 animation[0x1c]; u32 flags;
    u8 unknown28[0x18]; s16 position[3]; u8 unknown46[0x66];
    void *owner; u8 unknownB0[0x20]; u16 timer; u8 phase; u8 state;
};
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySongChecked(u32);
void CannonBattery_SpawnProjectile(void *, void *, s32);
void CannonBattery_SpawnMuzzleFlash(void *, void *, s32);
void sub_08199B84(struct CallbackSlot *slot)
{
    void *owner = slot->owner;
    AnimData_SelectPaletteBank(slot->animation, 0x8f);
    CannonBattery_SpawnProjectile(owner, slot->position, slot->flags & 4);
    CannonBattery_SpawnMuzzleFlash(owner, slot->position, slot->flags & 4);
    { u32 state = 2; slot->state = state;
    slot->phase = 0;
    slot->timer = 0; }
    Sound_PlaySongChecked(0x3fd);
}
