#include "global.h"

struct S {
    u8 f[0x1ba];
    u16 t;
};
void Sound_PlaySong(u32);
void sub_08055BE0(struct S *);
void Task_Kill(struct S *);

void sub_08055D38(struct S *p)
{
    u16 *t = &p->t;
    if (*t == 0)
        Sound_PlaySong(0x4EC);
    (*t)++;
    if (*t > 0x59) {
        sub_08055BE0(p);
        Task_Kill(p);
    }
}
