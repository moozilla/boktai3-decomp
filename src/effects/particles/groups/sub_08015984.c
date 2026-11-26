#include "global.h"
struct Config {
    u16 v[20];
};
int Div(int, int);
void AnimatedParticles_Create(int, void *, struct Config *);
void AnimatedParticles_EmitRing(void *pos, int count, int type, int flags, int speed,
                                int horizontal, int start, int decay)
{
    struct Config s;
    int step, i;
    s.v[0] = type;
    s.v[1] = flags;
    s.v[3] = speed;
    s.v[4] = 0;
    s.v[5] = 0;
    s.v[6] = horizontal;
    s.v[7] = 0;
    step = Div(256, count);
    s.v[9] = 0;
    s.v[8] = 0;
    s.v[12] = start;
    s.v[13] = 0;
    s.v[14] = decay;
    s.v[15] = 0;
    s.v[16] = start;
    s.v[17] = 0;
    s.v[18] = decay;
    s.v[19] = 0;
    s.v[11] = 10;
    s.v[10] = 0;
    for (i = 0; i < count; i++) {
        AnimatedParticles_Create(1, pos, &s);
        s.v[10] += step;
    }
}
