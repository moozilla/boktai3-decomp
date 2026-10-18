#include "global.h"
#include "directional_interaction.h"
struct SegmentDisplay {
    u8 unknown0[0x57e];
    u16 selection;
    u8 unknown580[0x2c];
    u32 selectionScript;
};
void SegmentDisplay_CallSelectionScript(struct SegmentDisplay *p)
{
    if (p->selectionScript) {
        s32 value;
        struct DirectionalScriptArgs args;
        args.count = 1;
        args.values = &value;
        value = p->selection;
        Script_Call(p->selectionScript, &args);
    }
}
