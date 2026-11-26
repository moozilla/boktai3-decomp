#include "global.h"
struct Config {
    u16 v[20];
};
void AnimatedParticles_Create(int, void *, struct Config *);
void AnimatedParticles_EmitDirectional(int count, int type, int flags, int speed, int yaw,
                                       int yawRange, int horizontal, int horizontalRange,
                                       int vertical, int verticalRange, void *pos)
{
    struct Config s;
    s.v[0] = type;
    s.v[1] = flags;
    s.v[3] = speed;
    s.v[4] = vertical;
    s.v[5] = verticalRange;
    s.v[6] = horizontal;
    s.v[7] = horizontalRange;
    s.v[8] = 60;
    s.v[9] = 8;
    s.v[10] = yaw;
    s.v[11] = yawRange;
    s.v[12] = 255;
    s.v[13] = 0;
    s.v[14] = 255;
    s.v[15] = 0;
    s.v[16] = 255;
    s.v[17] = 0;
    s.v[18] = 255;
    s.v[19] = 0;
    AnimatedParticles_Create(count, pos, &s);
}
