#include "global.h"
void sub_08018D84(void *);
void *OrbitingParticles_Create(void *, int, int, int, int);
void SceneCommands_CreateEffect(u8 *p, void *unused, s16 *message)
{
    struct {
        s16 x, y, z;
    } v;
    int a, b, c, d;
    v.x = message[4];
    v.y = message[5];
    v.z = message[6];
    a = message[7];
    b = message[8];
    c = message[9];
    d = message[10];
    if (*(void **)(p + 0x58)) {
        sub_08018D84(*(void **)(p + 0x58));
        *(void **)(p + 0x58) = 0;
    }
    *(void **)(p + 0x58) = OrbitingParticles_Create(&v, a, b, c, d);
}
