#include "global.h"
struct S {
    u8 f[0x770];
    u32 a770;
    u8 g[8];
    void (*a77c)(void *);
};
extern struct S *gUnk_02000264;
void Sound_PauseChannel12(u32);
void SparkleEmitter_UpdateDraining(void *);
void SparkleEmitter_StopEmitting(void)
{
    struct S *p = gUnk_02000264;
    if (p != 0) {
        void (*f)(void *);
        Sound_PauseChannel12(8);
        f = SparkleEmitter_UpdateDraining;
        p->a77c = f;
        p->a770 = 0;
    }
}
