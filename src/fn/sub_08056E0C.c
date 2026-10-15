#include "shake_icon_display.h"
s32 ShakeIconDisplay_Initialize(struct ShakeIconDisplay *display)
{
    struct ProjectedDisplayResource *resource = ScriptProc_Start(0xCB05, 0x1289);
    struct ProjectedDisplayResource *header, *savedHeader;
    struct ContactPosition *position;
    u32 variant;
    if (!resource)
        return -1;
    display->resource = *resource;
    header = &display->resource;
    ResourceHeader_Relocate(header, resource);
    {
        u32 found = Script_SeekToKeyword('t');
        savedHeader = header;
        if (found)
            variant = Script_GetValue();
        else
            variant = 0;
    }
    if (Script_SeekToKeyword('I'))
        display->lifetime = Script_GetValue();
    else
        display->lifetime = 60;
    display->update = ShakeIconDisplay_TickShake;
    position = &display->position;
    position->x = 0;
    display->position.y = 0;
    display->position.z = 0;
    sub_0821983C(&display->animation, savedHeader, (u16)(variant + 75), 16, 1, 0, 60, position);
    {
        u32 found = Script_SeekToKeyword('p');
        if (found)
            display->completionScriptId = Script_GetValue();
        else
            display->completionScriptId = found;
    }
    return 0;
}
