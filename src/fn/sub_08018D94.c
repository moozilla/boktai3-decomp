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
void Video_SetBlendControl(int, int, int);
void Video_SetBlendAlpha(int, int);
void Video_SetBlendBrightness(int);
void Task_Kill(void *);
int BlendTransition_Update(struct Blend *p)
{
    int first;
    Video_SetBlendControl(p->mode, p->source, p->dest);
    first = p->initial[0] + Div(p->delta[0] * p->time, p->duration);
    if (p->mode == 1)
        Video_SetBlendAlpha(first, p->initial[1] + Div(p->delta[1] * p->time, p->duration));
    else
        Video_SetBlendBrightness(first);
    if (p->time >= p->duration)
        Task_Kill(p);
    p->time++;
    return 0;
}
