#include "global.h"
#include "record_menu.h"
#define RecordMenu_UpdateRecords   sub_0810F544
#define RecordMenu_DrawRecords     sub_0810EA90
#define RecordMenu_UpdateSelection sub_0810F348
extern u16 gUnk_03005260[];
#define gMenuInput gUnk_03005260
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
void sub_0810EA90(struct RecordMenu *);
void Registry33FF_DrawActorValues(struct RecordMenu *);
void CharacterModel_UpdatePairedAnimations(void *, s32);
void PaletteFade_CycleMenuColors(void *);
void sub_0810F348(struct RecordMenu *);
void sub_0810F544(struct RecordMenu *menu)
{
    s32 i;
    u8 *panel;
    if (menu->redraw) {
        Bg_SetLayerMap(3, menu->backgroundMap, 2);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        sub_0810EA90(menu);
        Registry33FF_DrawActorValues(menu);
        menu->inputDelay = 0;
        menu->redraw = 0;
    }
    for (i = 0, panel = menu->panelStorage; i <= 1; panel += 0x850, i++)
        CharacterModel_UpdatePairedAnimations(panel, i);
    PaletteFade_CycleMenuColors(menu->textPanel);
    if (menu->inputDelay <= 59)
        menu->inputDelay++;
    else if (gUnk_03005260[1] & 11) {
        Sound_PlaySong(0xdd);
        {
            void (*next)(struct RecordMenu *) = sub_0810F348;
            u8 one = 1;
            menu->update = next;
            menu->redraw = one;
            menu->leaving = one;
        }
    }
}
