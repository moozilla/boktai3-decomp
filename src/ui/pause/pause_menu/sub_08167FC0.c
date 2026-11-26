#include "global.h"
void *ScriptProc_Start(u32, u32);
void Hud_SetupBackgroundLayer(void *, void *, u32, u32, void *);
void Hud_InitializePanelTransition(void *);
void Hud_InitializeThreeIcons(void *);

struct S08167FC0 {
    u8 unk0[0x18];
    void *res18;
    void *res1C;
    u8 unk20[8];
    void *res28;
    u8 *map2C;
    u8 *map30;
    u8 *map34;
    u8 *map38;
    u8 unk3C[0xA5E - 0x3C];
    u8 kind; /* 0xA5E */
    u8 unkA5F[0xA6D - 0xA5F];
    u8 flagA6D;
    u8 unkA6E[0xA94 - 0xA6E];
    u8 flagA94;
    u8 unkA95[0xAFC - 0xA95];
    u8 flagAFC;
};

/* Resources keyed 0x92B3 carry their map data 0x1B4 bytes in. */
static inline u8 *Resource_MapData(void *r)
{
    return (u8 *)r + 0x1B4;
}
void sub_08167FC0(struct S08167FC0 *p)
{
    p->res18 = ScriptProc_Start(0xC091, 0xA597);
    p->res1C = ScriptProc_Start(0xC091, 0xDB77);
    p->res28 = ScriptProc_Start(0xC091, 0xCE05);
    p->map2C = Resource_MapData(ScriptProc_Start(0x92B3, 0xEFDA));
    p->map30 = Resource_MapData(ScriptProc_Start(0x92B3, 0x771E));
    p->map30 = Resource_MapData(ScriptProc_Start(0x92B3, 0x500));
    p->map34 = Resource_MapData(ScriptProc_Start(0x92B3, 0x2E9));
    p->map38 = Resource_MapData(ScriptProc_Start(0x92B3, 0x6458));
    if (p->kind == 8)
        Hud_SetupBackgroundLayer(p, p->res28, 0, 0, p->map38);
    else
        Hud_SetupBackgroundLayer(p, p->res18, 0, 0, p->map2C);
    p->flagAFC = 0;
    p->flagA94 = 0;
    p->flagA6D = 0;
    Hud_InitializePanelTransition(p);
    Hud_InitializeThreeIcons(p);
}
