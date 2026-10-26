#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 ScriptedParticle_InitPool(u8 *);
void Task_Kill(u8 *);
void ScriptedParticle_UpdatePool(void);
void ScriptedParticle_DestroyPool(void);

u8 *ScriptedParticle_EnsurePool(void)
{
    u8 *p;
    if (gScriptedParticlePool != 0)
        return gScriptedParticlePool;
    p = Task_Create(0xa, 0x151c);
    if (p != 0) {
        Task_SetCallbacks(p, ScriptedParticle_UpdatePool, ScriptedParticle_DestroyPool);
        if (ScriptedParticle_InitPool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
