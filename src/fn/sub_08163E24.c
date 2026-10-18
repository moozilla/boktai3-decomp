#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ParticlePool_Init(u8 *);
void Task_Kill(u8 *);
void ParticlePool_OnUpdate(void);
void ParticlePool_OnDestroy(void);
/* Creates the 0x43C-byte task that owns 16 sprites (0x3C bytes each from
   +0x74; its destructor frees them). Scene NPCs and kind-29 characters keep
   the result (tentative name). */
u8 *ParticlePool_Create(void)
{
    u8 *p = Task_Create(10, 0x43c);
    if (p == 0)
        return 0;
    Task_SetCallbacks(p, ParticlePool_OnUpdate, ParticlePool_OnDestroy);
    if (ParticlePool_Init(p) < 0) {
        Task_Kill(p);
        return 0;
    }
    return p;
}
