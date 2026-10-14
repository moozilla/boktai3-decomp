#include "global.h"
#include "engine.h"

struct AnimData08197AE0 {
    u8 pad0[6];
    u16 id;
    u8 pad8[4];
    u8 *tiles;
};
struct Anim08197AE0 {
    u8 pad0[0xc];
    struct AnimData08197AE0 *data;
};
extern u8 *gUnk_030042E4;
void *sub_0819792C(void *, void *, u32, s32, s32);
void sub_08197A50(void *);
void sub_08197A0C(void *);

void sub_08197AE0(struct Anim08197AE0 *anim, u32 hold)
{
    void *task = sub_0819792C(anim, NULL, hold, 0, 0);
    if (task) {
        Task_SetCallbacks(task, sub_08197A50, sub_08197A0C);
        {
            u16 id = 0x192;
            u8 *tiles = gUnk_030042E4 + 0x3240;
            anim->data->id = id;
            anim->data->tiles = tiles;
        }
    }
}
