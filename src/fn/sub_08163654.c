#include "global.h"
#include "runtime.h"
struct P {
    u8 f0[0x33C];
    u16 a;
    s16 b;
    u8 x;
    u8 c;
    u8 e;
    u8 f;
    u8 g[2];
    u16 d;
};
extern u32 gEngineFlags;
void GameOver_UpdateLetterAnimation(struct P *);
void SceneLifecycle_RequestRestart(s32);
void Task_Kill(struct P *);
static inline void andm(u32 *a, u32 m)
{
    *a &= m;
}
void GameOver_UpdateExit(struct P *p)
{
    GameOver_UpdateLetterAnimation(p);
    if (p->c == 8) {
        andm(&gEngineFlags, ~0x400);
        gPlayTimeCounterEnabled = 0;
        SceneLifecycle_RequestRestart(p->f);
        Task_Kill(p);
    }
}
