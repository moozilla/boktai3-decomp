#include "global.h"
#include "scene_vec.h"
#define TileTrigger_SetStateFromScript sub_081082DC
struct TileTrigger {
    u32 flags;
    u8 unknown4[0x70];
    u8 animation[0x10];
    struct SceneVec position;
    u32 key;
    u8 unknown90[0x28];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
struct TileTrigger *Pool1B8_FindByName(u32);
s32 Script_GetKeywordValue(u32, s32);
void sub_081082DC(void)
{
    s32 mode = Script_GetKeywordValue(0x73, 0);
    struct TileTrigger *p = Pool1B8_FindByName(Script_GetKeywordValue(0x69, 0));
    if (!p)
        return;
    switch (mode) {
        case 0:
            if (!(p->triggerFlags & 0x40)) {
                {
                    u32 flags = p->triggerFlags | 0x40;
                    u16 mask = 0xFFDF;
                    p->triggerFlags = flags & mask;
                }
                p->state = 6;
                p->forced = 1;
            }
            break;
        case 1:
            if (!(p->triggerFlags & 0x20)) {
                {
                    u32 flags = p->triggerFlags | 0x20;
                    u16 mask = 0xFFBF;
                    p->triggerFlags = flags & mask;
                }
                p->state = 3;
                p->forced = mode;
            }
            break;
    }
}
