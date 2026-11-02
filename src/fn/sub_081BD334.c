#define gScriptPanelSource gPaletteBankData
#include "dark_loan_screen.h"
#include "render.h"

void *ScriptProc_Start(u16 key, u16 context);

void ScriptPanel_InitializeProcedure(struct DarkLoanScreen *panel)
{
    panel->procedureData = (u8 *)ScriptProc_Start(0x92B3, 0x2E9) + 0x1B4;
    CpuSet(gPaletteBankData + 0x7540, panel->snapshot, 0x04000008);
    panel->state = 0;
}
