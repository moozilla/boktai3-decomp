#include "monsters/centipede/centipede_body.h"

void Sprite_Remove(void *);

void CentipedeBody_RemoveSegmentSprites(struct TentacleBody *p)
{
    s32 i;
    for (i = 0; i < p->segmentCount; i++)
        Sprite_Remove(&p->segments[i]);
}
