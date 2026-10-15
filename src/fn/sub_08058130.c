#include "scene_panel.h"
void ScenePanel_SetChannelValue(struct ScenePanel *panel, s32 channel, u16 value)
{
    panel->channels[channel] = value;
    panel->channelsDirty = 1;
}
