#define PairedSceneActor_ApplyToActiveHalf sub_081B646C
#define PairedSceneActor_GetActiveMask sub_081B654C
#define PairedSceneActor_ApplyToSelectedHalves sub_081B6244
#define gRandomIndex gUnk_03005308
#define gRandomTable gUnk_0203B400
#include "global.h"
struct PairedSceneActor;
s32 sub_081B654C(struct PairedSceneActor *);
s32 sub_081B6244(struct PairedSceneActor *, u32, u32);
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
static inline s32 NextRandom(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(u16 *)(gUnk_03005308 * 2 + (u32)gUnk_0203B400);
}
s32 sub_081B646C(struct PairedSceneActor *p, u32 value)
{
    switch (sub_081B654C(p)) {
    case 0: return 0;
    case 2: return sub_081B6244(p,2,value);
    default:
        if (!((NextRandom() >> 3) & 1))
            return sub_081B6244(p,2,value);
    case 1:
        return sub_081B6244(p,1,value);
    }
}
