#include "global.h"
struct Blend {
    u8 pad[24];
    u8 mode;
    s8 initial[2], delta[2];
    u8 pad1d[3];
    u16 source, dest;
    u32 time, duration;
};
int Div(int, int);
void sub_08217060(int, int, int);
void sub_08217074(int, int);
void sub_08217084(int);
void sub_0821A0C0(void *);
int BlendTransition_Update(struct Blend *p)
{
    int first;
    sub_08217060(p->mode, p->source, p->dest);
    first = p->initial[0] + Div(p->delta[0] * p->time, p->duration);
    if (p->mode == 1)
        sub_08217074(first, p->initial[1] + Div(p->delta[1] * p->time, p->duration));
    else
        sub_08217084(first);
    if (p->time >= p->duration)
        sub_0821A0C0(p);
    p->time++;
    return 0;
}
