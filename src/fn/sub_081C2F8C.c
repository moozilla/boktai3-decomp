#include "global.h"
#include "ram.h"
struct S {
    u8 f[0x770];
    u32 a770;
    u8 g[8];
    void (*a77c)(void *);
};
void Sound_PauseChannel12(u32);
void SparkleEmitter_UpdateDraining(void *);
void SparkleEmitter_StopEmitting(void)
{
    struct S *p = gSparkleEmitter;
    if (p != 0) {
        void (*f)(void *);
        Sound_PauseChannel12(8);
        f = SparkleEmitter_UpdateDraining;
        p->a77c = f;
        p->a770 = 0;
    }
}
