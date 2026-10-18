#include "global.h"
s32 SceneHud_UpdateBinaryChoice(void *);
void Sound_PlaySong(s32);
void sub_0822DA94(s32);
void SceneHud_BeginEquipmentSelection(void *);
void Hud_UpdateInventorySortConfirmation(u8 *p)
{
    s32 r = SceneHud_UpdateBinaryChoice(p);
    if (r == 0) {
        Sound_PlaySong(0xde);
        SceneHud_BeginEquipmentSelection(p);
    } else if (r == 1) {
        Sound_PlaySong(0xdd);
        sub_0822DA94(0);
        p[0x4388] = 0;
        p[0x4389] = 0;
        p[0x438a] = 0;
        SceneHud_BeginEquipmentSelection(p);
    }
}
