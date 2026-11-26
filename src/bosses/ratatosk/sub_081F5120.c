#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "bosses/ratatosk/ratatosk_facing.h"

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(u32);
void Ratatosk_BeginDirectionalReaction(struct Ratatosk *p)
{
    struct Ratatosk *p2 = p;
    if (p->actionChanged) {
        int angle;
        p->actionChanged = 0;
        if (p->locked)
            angle = p->savedAngle;
        else
            angle = p->targetAngle + 0x80;
        RATATOSK_SET_FACING(p, angle);
        SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 0x1d, 0, 1, 4);
        Sound_PlaySongChecked(0x55f);
    }
    p2->frameDamage = 0;
    p2->motionFlag = 0;
    {
        u32 value;
        u32 mask = 12;
        value = p->flags | mask;
        value |= 64;
        p->flags = value;
    }
}
