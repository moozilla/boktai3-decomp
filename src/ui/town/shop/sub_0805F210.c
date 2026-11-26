#include "ui/widgets/scene_input_panel.h"
#include "engine/video/palette_pulse.h"
void SceneInputPanel_TickHighlight(struct SceneInputHighlight *highlight)
{
    Palette_UpdateHighlightPulse(&highlight->colors[12], &highlight->phase);
}
