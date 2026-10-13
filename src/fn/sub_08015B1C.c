#include "global.h"
struct Config {
    u16 v[20];
};
void sub_080154EC(int, void *, struct Config *);
#define AnimatedParticles_EmitFan sub_08015B1C
void AnimatedParticles_EmitFan(void *pos, int yaw, int yawRange, int count, int type, int flags,
                               int speed, int horizontal, int start, int decay)
{
    struct Config s;
    s.v[0] = type;
    s.v[1] = flags;
    s.v[3] = speed;
    s.v[4] = 0;
    s.v[5] = 0;
    s.v[6] = horizontal;
    s.v[7] = 0;
    s.v[8] = 0;
    s.v[9] = 0;
    s.v[10] = yaw;
    s.v[11] = yawRange;
    s.v[12] = start;
    s.v[13] = 0;
    s.v[14] = decay;
    s.v[15] = 0;
    s.v[16] = start;
    s.v[17] = 0;
    s.v[18] = decay;
    s.v[19] = 0;
    sub_080154EC(count, pos, &s);
}
