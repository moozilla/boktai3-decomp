#include "objects/pile_driver/pile_driver.h"
void PileDriver_SetChannelValue(struct PileDriver *panel, s32 channel, u16 value)
{
    panel->channels[channel] = value;
    panel->channelsDirty = 1;
}
