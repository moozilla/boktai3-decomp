#include "global.h"
extern u8 *gUnk_02000134;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08068868(u8 *);
void Task_Kill(u8 *);
void SceneParticlePool_Update(void);
void SceneParticlePool_Destroy(void);
u8 *sub_08068874(void)
{
    u8 *r;
    if (gUnk_02000134 != 0)
        return gUnk_02000134;
    r = Task_Create(10, 0x2C4);
    gUnk_02000134 = r;
    if (r) {
        Task_SetCallbacks(r, SceneParticlePool_Update, SceneParticlePool_Destroy);
        if (sub_08068868(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
