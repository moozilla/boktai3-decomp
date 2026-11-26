#include "ui/widgets/scene_input_panel.h"
#include "ui/hud/scene_hud_map.h"
extern const u16 gUnk_0824DF28[];
void SceneInputPanel_SetMeterGlyph(struct SceneInputPanel *panel, s32 value, s32 kind)
{
    panel->valueLabel.position.x = ((gUnk_0824DEB8[kind - 1] + value) << 3) + 8;
    AnimSprite_SelectGlyph(&panel->meterGlyph, &panel->widgetResource, gUnk_0824DF28[value]);
}
