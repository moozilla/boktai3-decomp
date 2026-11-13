#include <stdint.h>
#ifndef __wasm__
#include <stdio.h>
#endif
void ParticlePool_Destroy(void *);
int port_test_discarded_return(void)
{
    struct { uint8_t padding[18]; uint16_t flags; } task;
    static const uint16_t cases[] = {0, 2, 0xfffe};
    unsigned i;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        task.flags = cases[i];
        ParticlePool_Destroy(&task);
        if (task.flags != (uint16_t)(cases[i] | 1)) return 1;
    }
    return 0;
}
#ifndef __wasm__
int main(void)
{
    int result = port_test_discarded_return();
    if (!result) puts("native: original ParticlePool_Destroy -> Task_Kill PASS");
    return result;
}
#endif
