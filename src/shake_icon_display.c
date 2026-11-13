#include "shake_icon_display.h"

void ShakeIconDisplay_TickShake(struct ShakeIconDisplay *display)
{
    if (display->stateFrame <= 5) {
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 offset = gRandomIndex * 2;
            s32 sample = *(u16 *)(offset + RANDOM_TABLE_ADDR);
            display->animation.position.x = ((sample >> 4) & 15) - 7;
        }
    } else {
        display->animation.position.x = 0;
        display->animation.position.y = 0;
    }
}

s32 ShakeIconDisplay_Tick(struct ShakeIconDisplay *display)
{
    u16 *frame;
    u32 value;
    display->update(display);
    frame = &display->stateFrame;
    value = *frame + 1;
    *frame = value;
    if ((u16)value >= display->lifetime) {
        if (display->completionScriptId)
            Script_Call(display->completionScriptId, 0);
        Task_Kill(display);
    }
}

u32 ShakeIconDisplay_Destroy(struct ShakeIconDisplay *display)
{
    Anim_Remove(&display->animation);
    return 0;
}

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
    Widget_InitLinked(&display->animation, savedHeader, (u16)(variant + 75), 16, 1, 0, 60,
                      position);
    {
        u32 found = Script_SeekToKeyword('p');
        if (found)
            display->completionScriptId = Script_GetValue();
        else
            display->completionScriptId = found;
    }
    return 0;
}

struct ShakeIconDisplay *ShakeIconDisplay_CreateFromScript(void)
{
    struct ShakeIconDisplay *display = Task_Create(9, sizeof(struct ShakeIconDisplay));
    if (display) {
        Task_SetCallbacks(display, (void (*)(void *))ShakeIconDisplay_Tick,
                          (void (*)(void *))ShakeIconDisplay_Destroy);
        if (ShakeIconDisplay_Initialize(display) != 0) {
            Task_Kill(display);
            return 0;
        }
    }
    return display;
}
