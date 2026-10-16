#include "global.h"
#include "engine.h"

struct Vec08197890 {
    u16 x, y, z, w;
};
void sub_08197470(void *);
void sub_081975B0(void *);
void Nidhoggr_ReadOptions(void *);
s32 sub_081976E4(void *, u16, u16);
void SceneMessages_RegisterReceiver(void *, u32, u32);
void sub_08195A4C(void *, u32);

void *sub_08197890(u16 id, u16 arg)
{
    u8 *t = Task_Create(8, 0x72fc);
    if (t) {
        *(u32 *)(t + 0x4c) = id;
        Task_SetCallbacks(t, sub_08197470, sub_081975B0);
        Nidhoggr_ReadOptions(t);
        if (sub_081976E4(t, id, arg) < 0) {
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
        sub_08195A4C(t, 0xd);
    }
    return t;
}
