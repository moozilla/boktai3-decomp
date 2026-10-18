#include "global.h"
#define SceneActorState_StepTableByKind sub_08022D68
#define SceneActorState_StepTableByte sub_08022D24

extern const u8 gUnk_08604DC4[], gUnk_08604DE4[], gUnk_08604E04[], gUnk_08604E24[];
void sub_08022D24(u8 *p, const u8 *tbl);

int sub_08022D68(u8 *p)
{
    const u8 *tbl;
    switch (*(s32 *)(p + 0x358)) {
    case 0:
        tbl = gUnk_08604DC4;
        break;
    case 1:
        tbl = gUnk_08604DE4;
        break;
    case 2:
        tbl = gUnk_08604E04;
        break;
    case 3:
        tbl = gUnk_08604E24;
        break;
    default:
        goto fail;
    }
    sub_08022D24(p, tbl);
    return 1;
fail:
    return 0;
}
