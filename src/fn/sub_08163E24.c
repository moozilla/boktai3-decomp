#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08163E0C(u8 *);
void Task_Kill(u8 *);
void sub_08163BD4(void);
void sub_08163BE0(void);
#define ParticlePool_Create sub_08163E24
/* Creates the 0x43C-byte task that owns 16 sprites (0x3C bytes each from
   +0x74; its destructor frees them). Scene NPCs and kind-29 characters keep
   the result (tentative name). */
u8 *ParticlePool_Create(void)
{
    u8 *p = Task_Create(10, 0x43c);
    if (p == 0)
        return 0;
    Task_SetCallbacks(p, sub_08163BD4, sub_08163BE0);
    if (sub_08163E0C(p) < 0) {
        Task_Kill(p);
        return 0;
    }
    return p;
}
