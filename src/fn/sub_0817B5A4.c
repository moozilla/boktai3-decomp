#include "global.h"
extern const u32 gUnk_08611C5C[];
void sub_0824923C(void *, u32);
void SceneActors_DispatchSubtypeHandler(u8 *p)
{
    sub_0824923C(p, gUnk_08611C5C[p[0xae]]);
}
