// COMPILER: old_agbcc
#include "global.h"
#include "monsters/centipede/centipede_segment_animation.h"

/* Centipede_SetSegmentPosition: Copies a position into an indexed segment entry used by the position trail */

void Centipede_SetSegmentPosition(struct TentacleBodyAnimationView *g, s32 i, struct ContactPosition *pos)
{
    g->segments[i].position = *pos;
}
