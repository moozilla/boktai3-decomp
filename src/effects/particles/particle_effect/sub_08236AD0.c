#include "global.h"
struct Pair3B {
    u32 a, b;
};
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(u8 *), void (*)(u8 *));
void Task_Kill(u8 *);
void ParticleEffect_Update(u8 *);
void ParticleEffect_Destroy(u8 *);
s32 ParticleEffect_Initialize(u8 *, struct Pair3B *, u32, u32, u32, u32);
u8 *ActorBurst_Create(struct Pair3B *p, u32 a, u32 b, u32 c, u32 d)
{
    u8 *obj = Task_Create(8, 0x470);
    if (obj) {
        Task_SetCallbacks(obj, ParticleEffect_Update, ParticleEffect_Destroy);
        if (ParticleEffect_Initialize(obj, p, a, b, c, d) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
