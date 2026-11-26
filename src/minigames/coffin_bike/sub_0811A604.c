#include "global.h"
#include "minigames/coffin_bike/coffin_bike.h"
void MotionActor_DispatchRequestedState(void *);
void CoffinBike_UpdatePostState(void *);
u32 CoffinBike_UpdateTask(struct Glove *p)
{
    MotionActor_DispatchRequestedState(p);
    Callback_InvokeActor(p, p->update);
    CoffinBike_UpdatePostState(p);
    return 0;
}
