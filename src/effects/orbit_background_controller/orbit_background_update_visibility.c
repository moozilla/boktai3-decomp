#include "effects/orbit_background_controller/orbit_background.h"
static inline s32 LinkSession_Selected(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->selected;
}
static inline u8 LinkSession_SelectedStatus(void)
{
    s32 selected = LinkSession_Selected();
    if (!gLinkSession || selected < 0)
        return 0;
    return gLinkSession->status[selected];
}
static inline bool32 isSpecial(void)
{
    if (gPlayerActionPending == 1)
        return TRUE;
    return FALSE;
}
void OrbitBackground_UpdateVisibility(struct OrbitBackground *background)
{
    if (gEngineFlags & 0x800) {
        if (LinkSession_SelectedStatus())
            background->enabled = 0;
        else if (!background->enabled) {
            OrbitBackground_ResetOrbit(background);
            background->enabled = 1;
        }
        if (background->enabled)
            Bg_EnableDisplay(0x200);
        else
            Bg_DisableDisplay(0x200);
    } else {
        if (isSpecial()) {
            if (!background->specialMode)
                background->specialMode = 1;
        } else if (background->specialMode) {
            background->specialMode = 0;
            if (background->enabled)
                OrbitBackground_ResetOrbit(background);
        }
        if (background->enabled && !background->specialMode)
            Bg_EnableDisplay(0x200);
        else
            Bg_DisableDisplay(0x200);
    }
}
