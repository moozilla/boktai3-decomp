#include "ui/hud/shake_icon_display.h"
#include "link/wireless/random.h"

void ShakeIconDisplay_TickShake(struct ShakeIconDisplay *display)
{
    if (display->stateFrame <= 5) {
        s32 sample = RANDOM_NEXT();

        display->animation.position.x = ((sample >> 4) & 15) - 7;
    } else {
        display->animation.position.x = 0;
        display->animation.position.y = 0;
    }
}

s32 ShakeIconDisplay_Tick(struct ShakeIconDisplay *display)
{
    display->update(display);
    if (++display->stateFrame >= display->lifetime) {
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
    u32 variant;

    if (!resource)
        return -1;
    display->resource = *resource;
    ResourceHeader_Relocate(&display->resource, resource);
    if (Script_SeekToKeyword('t'))
        variant = Script_GetValue();
    else
        variant = 0;
    if (Script_SeekToKeyword('I'))
        display->lifetime = Script_GetValue();
    else
        display->lifetime = 60;
    display->update = ShakeIconDisplay_TickShake;
    display->position.x = 0;
    display->position.y = 0;
    display->position.z = 0;
    AnimSprite_InitLinked(&display->animation, &display->resource, (u16)(variant + 75), 16, 1, 0,
                          60, &display->position);
    if (Script_SeekToKeyword('p'))
        display->completionScriptId = Script_GetValue();
    else
        display->completionScriptId = 0;
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
