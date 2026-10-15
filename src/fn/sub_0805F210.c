#include "scene_input_panel.h"
#define SceneInputPanel_TickHighlight sub_0805F210
void SceneInputPanel_TickHighlight(struct SceneInputHighlight *highlight)
{
    sub_0816381C(&highlight->colors[12], &highlight->phase);
}
