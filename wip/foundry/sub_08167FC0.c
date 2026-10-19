#define Hud_InitializePanelResources  sub_08167FC0
#define Hud_InitializePanelTransition sub_08167E64
#define Hud_InitializeThreeIcons      sub_08167F1C
#include "global.h"

/* HUD interpretation and descriptive names are tentative. */
struct Hud {
    u8 pad0[0x18];
    void *primary, *secondary;
    u8 pad20[8];
    void *alternate;
    void *primaryGraphics, *secondaryGraphics, *extraGraphics, *alternateGraphics;
    u8 pad3c[0xA22];
    u8 mode;
    u8 padA5f[0xE];
    u8 stateA6d;
    u8 padA6e[0x26];
    u8 stateA94;
    u8 padA95[0x67];
    u8 stateAfc;
};
void *ScriptProc_Start(u16, u16);
void sub_08163F14(struct Hud *, void *, int, int, void *);
void sub_08167E64(struct Hud *);
void sub_08167F1C(struct Hud *);
void sub_08167FC0(struct Hud *hud)
{
    hud->primary = ScriptProc_Start(0xc091, 0xa597);
    hud->secondary = ScriptProc_Start(0xc091, 0xdb77);
    hud->alternate = ScriptProc_Start(0xc091, 0xce05);
    hud->primaryGraphics = (u8 *)ScriptProc_Start(0x92b3, 0xefda) + 0x1b4;
    hud->secondaryGraphics = (u8 *)ScriptProc_Start(0x92b3, 0x771e) + 0x1b4;
    hud->secondaryGraphics = (u8 *)ScriptProc_Start(0x92b3, 0x500) + 0x1b4;
    hud->extraGraphics = (u8 *)ScriptProc_Start(0x92b3, 0x2e9) + 0x1b4;
    hud->alternateGraphics = (u8 *)ScriptProc_Start(0x92b3, 0x6458) + 0x1b4;
    if (hud->mode == 8)
        sub_08163F14(hud, hud->alternate, 0, 0, hud->alternateGraphics);
    else
        sub_08163F14(hud, hud->primary, 0, 0, hud->primaryGraphics);
    hud->stateAfc = 0;
    hud->stateA94 = 0;
    hud->stateA6d = 0;
    sub_08167E64(hud);
    sub_08167F1C(hud);
}
