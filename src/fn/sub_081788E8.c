#define Hud_RemovePrimaryAnimation PauseMenu_RemoveHeaderAnimation
#define Hud_RemoveTimerAnimations  PauseMenu_RemoveEquipmentSlotAnimations
#include "global.h"
#include "ram.h"
struct FieldHud {
    u8 unknown0[0xC88];
    u8 animation0[0x1A0];
    u8 animation1[1];
};
void PauseMenu_RemoveHeaderAnimation(struct FieldHud *);
void PauseMenu_RemoveEquipmentSlotAnimations(struct FieldHud *);
void Anim_Remove(void *);
s32 Hud_Destroy(struct FieldHud *hud)
{
    PauseMenu_RemoveHeaderAnimation(hud);
    PauseMenu_RemoveEquipmentSlotAnimations(hud);
    Anim_Remove(hud->animation0);
    Anim_Remove(hud->animation1);
    return (s32)(gFieldHud = 0);
}
