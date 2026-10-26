#include "proximity_icon.h"
s32 ProximityIcon_InitFromScript(struct ProximityIcon *icon)
{
    struct ProjectedDisplayResource resource, *original = ScriptProc_Start(0xCB05, 0x82F1);
    struct ContactPackedVector offset, position;
    u32 priority, frame;
    resource = *original;
    ResourceHeader_Relocate(&resource, original);
    if (Script_SeekToKeyword('p')) {
        icon->origin.x = Script_GetValue();
        icon->origin.y = Script_GetValue();
        icon->origin.z = Script_GetValue();
    } else {
        icon->origin.x = 0;
        icon->origin.y = 0;
        icon->origin.z = 0;
    }
    if (Script_SeekToKeyword('o')) {
        offset.x = Script_GetValue();
        offset.y = Script_GetValue();
        offset.z = Script_GetValue();
    } else {
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
    }
    if (Script_SeekToKeyword('I'))
        icon->radius = Script_GetValue();
    else
        icon->radius = 384;
    if (Script_SeekToKeyword('t'))
        icon->type = Script_GetValue();
    else
        icon->type = 0;
    if (Script_SeekToKeyword('R'))
        priority = Script_GetValue();
    else
        priority = 1;
    switch (icon->type) {
        case 0: {
            u32 value;
            if (Script_SeekToKeyword('d'))
                value = Script_GetValue();
            else
                value = 0;
            frame = value;
            break;
        }
        case 1: {
            u32 value;
            if (Script_SeekToKeyword('d'))
                value = Script_GetValue();
            else
                value = 0;
            frame = value + 4;
            break;
        }
        default:
            frame = 0;
            break;
    }
    position.x = offset.x + icon->origin.x;
    position.y = offset.y + icon->origin.y;
    position.z = offset.z + icon->origin.z;
    Widget_InitLinked(&icon->animation, &resource, (u16)frame, 0, (u8)priority, 0, 60, &position);
    if (gEngineFlags & 0x200)
        icon->animation.flags |= 1;
    else if (ProximityIcon_IsPlayerNearby(icon))
        icon->animation.flags &= ~1;
    else
        icon->animation.flags |= 1;
    return 0;
}
