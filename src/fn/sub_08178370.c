#include "global.h"
#include "ram.h"
struct FieldHud {
    u8 unknown0[0xA5C];
    u16 blocked;
    u8 mode;
    u8 unknownA5F[0x4EA];
    u8 cueState;
};
void InventoryPanel_StartCallbackBurst(struct FieldHud *, s32, s32, s32);
void Sound_PlaySong(u32);
void Hud_BeginModeSixCue(void)
{
    struct FieldHud *hud = gFieldHud;
    if (hud && !hud->blocked && hud->mode <= 2) {
        InventoryPanel_StartCallbackBurst(hud, 0, 0xE0, 0x90);
        hud->cueState = 6;
        Sound_PlaySong(0x382);
    }
}
