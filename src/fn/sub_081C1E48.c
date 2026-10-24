#include "global.h"
#include "script_utility.h"
// MATCHING_CONCESSION: register-pinning

struct ControlPanelCounter {
    u8 unknown0[0x590];
    u16 multiplier;
    u8 unknown592[0x5A4 - 0x592];
    u32 multiplierScript;
    u8 unknown5A8[0x5B0 - 0x5A8];
    u16 accumulator;
    u16 threshold;
};
extern u8 *gUnk_02000580;
u32 ControlPanel_GetInputMultiplier(struct ControlPanelCounter *);
void ControlPanel_ApplyMultiplierMode(struct ControlPanelCounter *);

s32 sub_081C1E48(struct ControlPanelCounter *p)
{
    s32 step;
    u32 multiplier;
    u16 *level = (u16 *)(gUnk_02000580 + 0x522);
    if (*level == 0)
        step = 0;
    else if (*level <= 3)
        step = 1;
    else {
        register s32 factor asm("r7") = *level - 2;
        step = (factor * (*level - 3)) >> 1;
    }
    multiplier = ControlPanel_GetInputMultiplier(p);
    if (p->multiplier != multiplier) {
        u32 script = p->multiplierScript;
        if (script) {
            s32 value;
            struct ScriptUtilityArguments args;
            args.count = 1;
            args.values = &value;
            value = multiplier;
            Script_Call(script, &args);
        }
        p->multiplier = multiplier;
        ControlPanel_ApplyMultiplierMode(p);
    }
    {
        u32 sum = p->accumulator = p->multiplier * step + p->accumulator;
        if ((u16)sum >= p->threshold) {
            s32 n = 0;
            while (p->accumulator >= p->threshold) {
                n++;
                p->accumulator -= p->threshold;
            }
            return n;
        }
    }
    return 0;
}
