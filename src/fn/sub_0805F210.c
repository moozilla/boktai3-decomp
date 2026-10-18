#include "scene_input_panel.h"
void SceneInputPanel_TickHighlight(struct SceneInputHighlight *highlight)
{
    Palette_UpdateHighlightPulse(&highlight->colors[12], &highlight->phase);
}
