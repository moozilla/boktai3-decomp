#include "wireless_link_indicator.h"
#define WirelessLinkIndicator_SelectStrengthFrame sub_080483AC
void WirelessLinkIndicator_SelectStrengthFrame(struct WirelessLinkIndicator *input, s32 strength)
{
    struct WirelessLinkIndicator *indicator = input;
    u32 tick;
    u32 frame;
    if (strength > 228) {
        u32 elapsed = indicator->animationTick;
        s32 phase = 31;
        phase &= elapsed;
        frame = 34;
        tick = elapsed;
        if (phase > 7) {
            frame = 35;
            if (phase > 15) {
                frame = 37;
                if (phase <= 23)
                    frame = 36;
            }
        }
    } else if (strength > 126) {
        u32 elapsed = indicator->animationTick;
        s32 phase = 31;
        phase &= elapsed;
        frame = 34;
        tick = elapsed;
        if (phase > 9) {
            frame = 36;
            if (phase <= 19)
                frame = 35;
        }
    } else if (strength > 24) {
        u32 elapsed = indicator->animationTick;
        s32 phase = 31;
        phase &= elapsed;
        frame = 33;
        tick = elapsed;
        if (phase > 9) {
            frame = 35;
            if (phase <= 19)
                frame = 34;
        }
    } else {
        s32 phase;
        tick = indicator->animationTick;
        phase = tick & 31;
        frame = 33;
        if (phase <= 15)
            frame = 34;
    }
    {
        u32 nextTick = tick + 1;
        indicator->animationTick = nextTick;
    }
    if (frame != indicator->selectedFrame) {
        indicator->selectedFrame = frame;
        Anim_LoadKeyframe(&indicator->animation, &indicator->resource, frame, 0);
    }
}
