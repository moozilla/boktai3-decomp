#include "wireless_link_indicator.h"

struct WirelessLinkIndicator *WirelessLinkIndicator_Get(void)
{
    return gWirelessLinkIndicator;
}

void WirelessLinkIndicator_SelectStrengthFrame(struct WirelessLinkIndicator *input, s32 strength)
{
    struct WirelessLinkIndicator *indicator = input;
    u32 tick;
    u32 frame;
    if (strength > 228) {
        u32 elapsed = indicator->animationTick;
        s32 phase = 31;
        phase &= elapsed;
        frame = 34;
        tick = elapsed;
        if (phase > 7) {
            frame = 35;
            if (phase > 15) {
                frame = 37;
                if (phase <= 23)
                    frame = 36;
            }
        }
    } else if (strength > 126) {
        u32 elapsed = indicator->animationTick;
        s32 phase = 31;
        phase &= elapsed;
        frame = 34;
        tick = elapsed;
        if (phase > 9) {
            frame = 36;
            if (phase <= 19)
                frame = 35;
        }
    } else if (strength > 24) {
        u32 elapsed = indicator->animationTick;
        s32 phase = 31;
        phase &= elapsed;
        frame = 33;
        tick = elapsed;
        if (phase > 9) {
            frame = 35;
            if (phase <= 19)
                frame = 34;
        }
    } else {
        s32 phase;
        tick = indicator->animationTick;
        phase = tick & 31;
        frame = 33;
        if (phase <= 15)
            frame = 34;
    }
    {
        u32 nextTick = tick + 1;
        indicator->animationTick = nextTick;
    }
    if (frame != indicator->selectedFrame) {
        indicator->selectedFrame = frame;
        Anim_LoadKeyframe(&indicator->animation, &indicator->resource, frame, 0);
    }
}

/* WirelessLinkIndicator_UpdateChildStatus: Updates the indicator from child strength slot zero and shows it in child mode; hides it in parent mode */

void WirelessLinkIndicator_UpdateChildStatus(struct WirelessLinkIndicator *indicator)
{
    struct RfuLinkStatus *status = gRfuLinkStatus;
    if (!status->parentChild) {
        WirelessLinkIndicator_SelectStrengthFrame(indicator, status->strength[0]);
        indicator->animation.flags &= ~1;
    } else
        indicator->animation.flags |= 1;
}

void WirelessLinkIndicator_UpdateLinkStatus(struct WirelessLinkIndicator *indicator)
{
    if (!Wireless_GetStatus()) {
        s32 strength = 255;
        if (Wireless_IsSessionConnected()) {
            struct RfuLinkStatus *status = gRfuLinkStatus;
            if (!status->parentChild) {
                s32 slot = Wireless_GetConnectedParentSlot();
                strength = gRfuLinkStatus->strength[slot];
                WirelessLinkIndicator_SelectStrengthFrame(indicator, strength);
            } else {
                s32 index = 0;
                s32 slots;
                u8 *values;
                u32 connected;
                slots = status->connSlotFlag;
                connected = 1;
                values = status->strength;
                do {
                    if ((slots >> index) & connected) {
                        s32 current = values[index];
                        if (strength > current) {
                            strength = current;
                        }
                    }
                    ++index;
                } while (index <= 3);
                if (!gRfuLinkStatus->connSlotFlag) {
                    WirelessLinkIndicator_SelectStrengthFrame(indicator, 0);
                } else {
                    WirelessLinkIndicator_SelectStrengthFrame(indicator, strength);
                }
            }
            indicator->animation.flags &= ~1;
        } else {
            indicator->animation.flags |= 1;
        }
    }
}

s32 WirelessLinkIndicator_Update(struct WirelessLinkIndicator *indicator)
{
    WirelessLinkIndicator_UpdateLinkStatus(indicator);
    ++indicator->frame;
    return 0;
}

s32 WirelessLinkIndicator_Destroy(struct WirelessLinkIndicator *indicator)
{
    Anim_Remove(&indicator->animation);
    gWirelessLinkIndicator = NULL;
    return 0;
}

s32 Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);

s32 WirelessLinkIndicator_Initialize(struct WirelessLinkIndicator *indicator)
{
    const struct ProjectedDisplayResource *resource;
    struct ProjectedDisplayResource *relocated;
    s32 result;
    gWirelessLinkIndicator = indicator;
    resource = ScriptProc_Start(0xCB05, 0x7C8B);
    if (resource) {
        indicator->resource = *resource;
        relocated = &indicator->resource;
        ResourceHeader_Relocate(relocated, (void *)resource);
        Widget_InitLinked(&indicator->animation, relocated, 34, 17, 0, 0, 60, NULL);
        result = 0;
    } else
        result = -1;
    return result;
}

struct WirelessLinkIndicator *WirelessLinkIndicator_Create(void)
{
    struct WirelessLinkIndicator *indicator = WirelessLinkIndicator_Get();
    if (indicator)
        return indicator;
    indicator = Task_Create(11, sizeof(*indicator));
    if (indicator) {
        Task_SetCallbacks(indicator, (void (*)(void *))WirelessLinkIndicator_Update,
                          (void (*)(void *))WirelessLinkIndicator_Destroy);
        if (WirelessLinkIndicator_Initialize(indicator) < 0) {
            Task_Kill(indicator);
            return 0;
        }
    }
    return indicator;
}

void WirelessLinkIndicator_SetPosition(u32 x, u32 y)
{
    struct WirelessLinkIndicator *indicator = WirelessLinkIndicator_Create();
    if (indicator) {
        struct ProjectedDisplayEntry *animation = &indicator->animation;
        animation->position.x = x;
        animation->position.y = y;
    }
}
