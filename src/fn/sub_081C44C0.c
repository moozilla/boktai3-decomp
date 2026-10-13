#include "global.h"

struct S {
    u8 pad[0x64];
    void (*f64)(void);
    u32 f68;
};
void Sound_PlaySong(u32);
void sub_081C45E0(void);

void sub_081C44C0(struct S *p)
{
    Sound_PlaySong(0x50F);
    p->f64 = sub_081C45E0;
    p->f68 = 0;
}
