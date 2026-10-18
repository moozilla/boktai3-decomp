#include "global.h"
/* ArmorEffect_Stop: Clears the effect update slot and invokes then clears its cleanup callback; effect identity is tentative */
#define ArmorEffect_Stop sub_08180898

struct S08180898 {
    u32 active;
    void (*callback)(struct S08180898 *);
};

s32 sub_08180898(struct S08180898 *s)
{
    s->active = 0;
    if (s->callback) {
        s->callback(s);
        s->callback = NULL;
    }
}
