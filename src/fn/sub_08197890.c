#include "global.h"
#include "engine.h"

struct Vec08197890 {
    u16 x, y, z, w;
};
void Nidhoggr_UpdateCutscene(void *);
void Nidhoggr_DestroyCutscene(void *);
void Nidhoggr_ReadOptions(void *);
s32 Nidhoggr_Init(void *, u16, u16);
void SceneMessages_RegisterReceiver(void *, u32, u32);
void Nidhoggr_SetBehavior(void *, u32);

void *Nidhoggr_CreateCutscene(u16 id, u16 arg)
{
    u8 *t = Task_Create(8, 0x72fc);
    if (t) {
        *(u32 *)(t + 0x4c) = id;
        Task_SetCallbacks(t, Nidhoggr_UpdateCutscene, Nidhoggr_DestroyCutscene);
        Nidhoggr_ReadOptions(t);
        if (Nidhoggr_Init(t, id, arg) < 0) {
            Task_Kill(t);
            return NULL;
        }
        {
            u16 tmp = *(u16 *)(t + 0x72a0);
            ((struct Vec08197890 *)(t + 0x7290))->x = tmp;
        }
        /* the original writes x three times (y and z never set) */
        ((struct Vec08197890 *)(t + 0x7290))->x = *(u16 *)(t + 0x72a2);
        ((struct Vec08197890 *)(t + 0x7290))->x = *(u16 *)(t + 0x72a4);
        *(struct Vec08197890 *)(t + 0x7298) = *(struct Vec08197890 *)(t + 0x7290);
        SceneMessages_RegisterReceiver(t + 0x18, *(u32 *)(t + 0x4c), 4);
        Nidhoggr_SetBehavior(t, 0xd);
    }
    return t;
}
