#define SceneSequence_UpdateRampDown64 sub_0816B200
#include "global.h"
struct SceneSequence { u8 unknown0[0xA4C]; u16 phaseTimer; };
void Hud_InitSelectionBounds(u32, u32);
void sub_0816ABC8(void *);
void sub_0816B200(struct SceneSequence *s)
{
    if (s->phaseTimer <= 15)
        Hud_InitSelectionBounds((16 - s->phaseTimer) * 4, 0);
    else
        Hud_InitSelectionBounds(0, 0);
    if (++s->phaseTimer > 15)
        sub_0816ABC8(s);
}
