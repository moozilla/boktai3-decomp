// COMPILER: old_agbcc
#include "global.h"
#include "monsters/centipede/centipede_segment_animation.h"

/* TentacleEnemy_SetSegmentPosition: Copies a position into an indexed segment entry used by the position trail */
#define TentacleEnemy_SetSegmentPosition sub_080ACCD4

void sub_080ACCD4(struct TentacleBodyAnimationView *g, s32 i, struct ContactPosition *pos)
{
    g->segments[i].position = *pos;
}
