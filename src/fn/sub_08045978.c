#include "pooled_motion.h"
void MotionPool_SetGraphics(struct MotionPoolParams *params, u32 resource, u32 frame, s32 tile)
{
    params->graphicResource = resource;
    params->frame = frame;
    params->tile = tile;
}
