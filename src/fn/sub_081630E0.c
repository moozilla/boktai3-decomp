#include "global.h"

struct P {
    u8 f00[0x341];
    u8 i;
};
extern const u32 gUnk_08610E44[];

void Callback_InvokeActor(struct P *, u32);
void sub_08163020(struct P *);

void GameOver_UpdateLetterAnimation(struct P *p)
{
    Callback_InvokeActor(p, gUnk_08610E44[p->i]);
    sub_08163020(p);
}
