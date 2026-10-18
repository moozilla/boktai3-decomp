#include "global.h"
#include "scene_pool.h"
#define ScenePool_ClampMotionSpeeds sub_0811CFAC
#define ScenePool_ClampForwardBias  sub_0811CF68
void sub_0811CF68(struct ScenePoolActor *);
s32 sub_0811EA78(struct ScenePoolActor *, s32);
void sub_0811CFAC(struct ScenePoolActor *s)
{
    s32 maximum = sub_0811EA78(s, s->maximumForwardSpeed);
    if (s->forwardSpeed > maximum)
        s->forwardSpeed = maximum;
    if (s->forwardSpeed < s->minimumForwardSpeed)
        s->forwardSpeed = s->minimumForwardSpeed;
    if (s->lateralSpeed > s->maximumLateralSpeed)
        s->lateralSpeed = s->maximumLateralSpeed;
    if (s->lateralSpeed < s->minimumLateralSpeed)
        s->lateralSpeed = s->minimumLateralSpeed;
    sub_0811CF68(s);
}
