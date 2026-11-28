#include "effects/directional_effect/spark_burst.h"

// confidence=low: moves each live spark by its velocity, switches its frame at tick 6 and
// hides it after tick 11; the burst ends when no spark is left.
void sub_08238FE0(struct PlayerSparkView *player)
{
    s32 live, i;

    if (!player->burst.active)
        return;
    live = 0;
    for (i = 0; i < 6; i++) {
        if (player->burst.sparks[i].active) {
            struct DirectionalSpark *spark = &player->burst.sparks[i];
            struct SpriteDrawable *drawable = &spark->drawable;

            if (++spark->age > 11) {
                drawable->flags |= 1;
                spark->active = 0;
            } else {
                if (spark->age == 6)
                    SpriteDrawable_SelectTileFrame(&spark->drawable, player->burst.resource,
                                                   spark->baseFrame + 1);
                drawable->position.x += spark->velocity.x;
                drawable->position.y += spark->velocity.y;
                drawable->position.z += spark->velocity.z;
                live++;
            }
        }
    }
    if (live == 0)
        player->burst.active = 0;
}
