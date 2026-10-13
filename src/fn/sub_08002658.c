#include "global.h"
#include "background_controller.h"

/* Destructor: clears the layer's slot. Returns u32 (no value) like the original. */
u32 sub_08002658(struct ScrollingBackground *p)
{
    /* The slot table is pointer-sized; a u32 view keeps the original access shape. */
    u32 *q = (u32 *)gUnk_02000010;
    u32 i = p->layer;
    q[i] = 0;
}
