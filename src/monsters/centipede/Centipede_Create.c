#include "monsters/centipede/centipede_body.h"

s32 Centipede_Init(void *);
void CentipedeBody_Destroy(void *);

void Centipede_Create(void)
{
    struct TentacleBody *body = Heap_Alloc(sizeof(*body));
    if (body != NULL) {
        Memory_Clear(body, sizeof(*body));
        if (Centipede_Init(body) < 0) {
            CentipedeBody_Destroy(body);
            Heap_Free(body);
        }
    }
}
