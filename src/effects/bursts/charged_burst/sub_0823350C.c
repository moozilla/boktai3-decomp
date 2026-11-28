#include "effects/bursts/charged_burst.h"

s32 Div(s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);

// confidence=low: pulls each live spark toward the target by 1/(9 - t) of the gap per frame,
// switches it to frame 9 at tick 4 and hides it after tick 5.
void sub_0823350C(struct ChargedBurst *burst)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (!burst->sparks[i].active)
            continue;
        if (++burst->sparks[i].timer > 5) {
            burst->sparks[i].sprite.flags |= 1;
            burst->sparks[i].active = 0;
        } else {
            s32 weight = 8 - burst->sparks[i].timer;
            s32 divisor = weight + 1;

            burst->sparks[i].sprite.position.x =
                Div(burst->sparks[i].sprite.position.x * weight + burst->core.position.x, divisor);
            burst->sparks[i].sprite.position.y =
                Div(burst->sparks[i].sprite.position.y * weight + burst->core.position.y, divisor);
            burst->sparks[i].sprite.position.z =
                Div(burst->sparks[i].sprite.position.z * weight + burst->core.position.z, divisor);
            if (burst->sparks[i].timer == 4)
                SpriteDrawable_SelectTileFrame(&burst->sparks[i], burst->sparkResource, 9);
        }
    }
}
