// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u8 *gUnk_030042E4;
int SceneActorGraphics_GetResourceId(void *);
void SceneActorGraphics_SelectResource(u8 *p)
{
    if (p[0x38e]) {
        int resource;
        p[0x38c] = 0;
        p[0x38e] = 0;
        *(int *)(p + 0x390) = SceneActorGraphics_GetResourceId(p);
        resource = SceneActorGraphics_GetResourceId(p);
        {
            int test = resource;
            asm("" : "+r"(test));
            if (test == 0)
                *(void **)(p + 0x288) = p + 0x5c4;
            else if (test == 1)
                *(void **)(p + 0x288) = p + 0x7c4;
            else {
                {
                    void *src = gUnk_030042E4 + resource * 32;
                    register void *dst asm("r1") = p + 0x3c4;
                    CpuSet(src, dst, 0x4000008);
                }
                *(void **)(p + 0x288) = p + 0x3c4;
            }
        }
    }
}
