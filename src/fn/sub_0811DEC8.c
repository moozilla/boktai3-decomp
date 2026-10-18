#include "global.h"
struct S {
    u32 a;
};
extern struct S *gUnk_02000208;
struct S *Modifier_GetTableTask(void)
{
    if (!gUnk_02000208)
        return 0;
    return &gUnk_02000208->a;
}
