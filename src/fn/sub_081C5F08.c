#include "challenge_result.h"

void ChallengeResult_LoadDigit(struct SceneInputWidget *digit, struct SceneInputResource *graphics,
                               u32 value)
{
    if (value <= 9)
        Anim_LoadKeyframe(digit, graphics, (u16)value, 0);
}
