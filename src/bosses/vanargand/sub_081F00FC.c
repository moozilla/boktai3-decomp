#include "global.h"
struct SceneActorTween {
    u8 pad0[0x90];
    u32 frame;
    u8 pad94[0xE];
    u8 progressFlag;
    u8 padA3;
    u32 progress;
    u8 padA8[6];
    u8 phase, finished, entered, initialize;
    u8 padB2[0x12];
    u32 timer;
    u8 padC8[0x38];
    void (*callback)(void);
    u8 pad104[0x4B8];
    u32 completed;
};
void SceneMover_InterpolateScreenX(void);
void SingletonSlot7_UpdateShakingDeparture(void);
u32 Trigger_ConsumePending(void *, u32);
static inline u8 ConsumeEntry(struct SceneActorTween *p)
{
    u8 *q = &p->entered;
    if (*q) {
        *q = 0;
        *(q - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
static inline void SetCallback(void (*v)(void), void (**a)(void))
{
    *a = v;
}
void SceneActors_AdvanceToHorizontalTween(struct SceneActorTween *p)
{
    if (ConsumeEntry(p) == 1) {
        SetCallback(SingletonSlot7_UpdateShakingDeparture, &p->callback);
        p->frame = 0;
        p->initialize = 1;
        p->progressFlag = 0;
        p->progress = 0;
    }
    if (p->finished) {
        SetCallback(SceneMover_InterpolateScreenX, &p->callback);
        p->frame = 0;
        p->initialize = 1;
        p->progressFlag = 0;
        p->progress = 0;
        Trigger_ConsumePending((u8 *)p + 0x118, 1);
    }
    p->timer++;
}
