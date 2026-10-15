#include "contact_swarm.h"
struct EntryManagerView {
    u8 unknown0[0x18];
    u16 count;
};
extern struct EntryManagerView *gUnk_02000110;
struct EntryManagerView *sub_0804FFEC(void);
void sub_0804FEFC(struct EntryManagerView *, s32, struct ContactPackedVector *, s32, s32, s32, s32);
void RepeatedEffect_AddFromScript(void)
{
    struct EntryManagerView *manager = gUnk_02000110;
    s32 kind, speed, height, flags, mode;
    struct ContactPackedVector position;
    if (!manager)
        manager = sub_0804FFEC();
    if (manager->count > 7)
        return;
    if (Script_SeekToKeyword('n'))
        kind = Script_GetValue();
    else
        kind = 0;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    if (Script_SeekToKeyword('s'))
        speed = Script_GetValue();
    else
        speed = 0;
    if (Script_SeekToKeyword('h'))
        height = Script_GetValue();
    else
        height = 0;
    if (Script_SeekToKeyword('f'))
        flags = Script_GetValue();
    else
        flags = 0;
    if (Script_SeekToKeyword('m'))
        mode = Script_GetValue();
    else
        mode = 0;
    sub_0804FEFC(manager, kind, &position, speed, height, flags, mode);
    manager->count++;
}
