#include "global.h"

void sub_080AB7BC(u8 *);
void sub_080AB6F4(u8 *);
void ActorTrail_UpdateTimedScale(u8 *);
void SpritePool_FlagEntries(u8 *);
void sub_080A9988(u8 *);
void sub_080AAA90(u8 *);
void sub_080AA8C4(u8 *);
void sub_080AB04C(u8 *);

s32 TentacleActor_UpdateComponents(u8 *p)
{
    s16 *timer;
    sub_080AB7BC(p);
    sub_080AB6F4(p);
    ActorTrail_UpdateTimedScale(p);
    SpritePool_FlagEntries(p);
    sub_080A9988(p);
    sub_080AAA90(p);
    sub_080AA8C4(p);
    sub_080AB04C(p);
    timer = (s16 *)(p + 0x8d6);
    if (*timer > 0)
        (*timer)--;
    return 1;
}
