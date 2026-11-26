#include "menus/menu_prompt/menu_prompt.h"
#include "engine/shared/player.h"
void Hud_InitSelectionBounds(u32, u32);
void SceneSequence_UpdateRampDown64(struct SceneHudState *s)
{
    if (s->pageFadeTimer <= 15)
        Hud_InitSelectionBounds((16 - s->pageFadeTimer) * 4, 0);
    else
        Hud_InitSelectionBounds(0, 0);
    if (++s->pageFadeTimer > 15)
        MenuPrompt_ResumeChild(s);
}
