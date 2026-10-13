#include "global.h"
struct Config {
    u16 v[20];
};
struct Vec {
    s16 x, y, z, pad;
};
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void sub_080154EC(int, void *, struct Config *);
#define AnimatedParticles_CreateFromScript sub_08015DE0
void AnimatedParticles_CreateFromScript(void)
{
    struct Config s;
    struct Vec pos;
    int count = sub_0821ABA8('n', 1);
    if (Script_SeekToKeyword('p')) {
        pos.x = Script_GetValue();
        pos.y = Script_GetValue();
        pos.z = Script_GetValue();
    } else {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
    }
    s.v[0] = sub_0821ABA8('t', 0);
    s.v[1] = sub_0821ABA8('f', 0);
    s.v[3] = sub_0821ABA8('a', 64);
    if (Script_SeekToKeyword('s')) {
        s.v[4] = Script_GetValue();
        s.v[5] = Script_GetValue();
        s.v[6] = Script_GetValue();
        s.v[7] = Script_GetValue();
    } else {
        s.v[4] = 0;
        s.v[5] = 0;
        s.v[6] = 0;
        s.v[7] = 0;
    }
    if (Script_SeekToKeyword('d')) {
        s.v[8] = Script_GetValue();
        s.v[9] = Script_GetValue();
        s.v[10] = Script_GetValue();
        s.v[11] = Script_GetValue();
    } else {
        s.v[8] = 0;
        s.v[9] = 0;
        s.v[10] = 0;
        s.v[11] = 0;
    }
    if (Script_SeekToKeyword('c')) {
        s.v[12] = Script_GetValue();
        s.v[13] = Script_GetValue();
        s.v[14] = Script_GetValue();
        s.v[15] = Script_GetValue();
        s.v[16] = Script_GetValue();
        s.v[17] = Script_GetValue();
        s.v[18] = Script_GetValue();
        s.v[19] = Script_GetValue();
    } else {
        s.v[12] = 255;
        s.v[13] = 0;
        s.v[14] = 255;
        s.v[15] = 0;
        s.v[16] = 255;
        s.v[17] = 0;
        s.v[18] = 255;
        s.v[19] = 0;
    }
    sub_080154EC(count, &pos, &s);
}
