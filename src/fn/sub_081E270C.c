#include "global.h"
#include "input.h"
// Provisional name: TimedPresentation_Update
struct TimedSequence {
    u8 pad[0x18];
    s32 state, timer, exitTimer, minShift, fadeShift, duration, unused;
    void *handle;
};
void Menu_StartFadeOut(s32);
s32 Heap_IsEndGuardValid(void);
void Script_Call(void *, void *);
void Task_Kill(void *);
// Wait for the sequence duration or an eligible skip, then finish its exit fade.
void TimedPresentation_Update(struct TimedSequence *p)
{
    switch (p->state) {
        case 0:
            if (p->timer > p->duration - (1 << p->fadeShift)) {
                p->state = 1;
                Menu_StartFadeOut(p->fadeShift);
            } else if (p->timer > (1 << p->minShift) && Heap_IsEndGuardValid() &&
                       (gPadState[1] & 1)) {
                p->state = 1;
                Menu_StartFadeOut(p->fadeShift);
            }
            break;
        case 1:
            if (p->exitTimer > (1 << p->fadeShift)) {
                Script_Call(p->handle, 0);
                Task_Kill(p);
            }
            p->exitTimer++;
            break;
        case 2:
            break;
    }
    p->timer++;
}
