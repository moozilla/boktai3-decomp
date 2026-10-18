#include "global.h"
#include "scene_pool.h"
void ScenePool_ClampForwardBias(struct ScenePoolActor *s)
{
    if (s->forwardBias > s->maximumForwardBias)
        s->forwardBias = s->maximumForwardBias;
    if (s->forwardBias < s->minimumForwardBias)
        s->forwardBias = s->minimumForwardBias;
    if ((s->boundsFlags & 0x200) && s->biasClampMode != 2)
        s->forwardBias = s->minimumForwardBias;
}
