#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);

void Ratatosk_UpdateDirectionalAnimation(struct Ratatosk *self, s32 frame)
{
    struct Ratatosk *actor = self;

    if (self->actionChanged) {
        self->actionChanged = 0;
        if (self->locked)
            Ratatosk_SetFacing(self, self->savedAngle);
        SceneActors_StartDirectionalAnimation(self, self->animationData, self->animation, 0, 1, 1,
                                              4);
    } else {
        SceneActors_StartDirectionalAnimation(self, self->animationData, self->animation, 0, 1, 1,
                                              1);
    }
    actor->scaleX = 0;
    actor->scaleZ = 0;
    actor->scaleMode = 0;
    Ratatosk_SetFlags(self, 0x100000);
    if (!(u8)Callback_InvokeActor(self, self->actionTryUpdate)) {
        if (frame >= actor->interval && !self->locked)
            self->actionFinished = 1;
        if (!self->actionPhase)
            Ratatosk_SetFlags(self, 0x10);
        Ratatosk_SetFlags(self, 0x40);
    }
}
