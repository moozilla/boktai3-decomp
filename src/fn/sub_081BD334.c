#define gScriptPanelSource gPaletteBankData
#include "global.h"
#include "render.h"

struct ScriptPanel {
    u8 pad0[0x8E4];
    void *procedureData;
    u8 pad8E8[0x27];
    u8 state;
    u32 snapshot[8];
};
void *ScriptProc_Start(u16 key, u16 context);

void ScriptPanel_InitializeProcedure(struct ScriptPanel *panel)
{
    panel->procedureData = (u8 *)ScriptProc_Start(0x92B3, 0x2E9) + 0x1B4;
    CpuSet(gPaletteBankData + 0x7540, panel->snapshot, 0x04000008);
    panel->state = 0;
}
