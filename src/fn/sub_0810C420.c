#include "global.h"
#include "area_effect_link.h"
struct LinkPanel {
    u8 pad0[0xf44];
    u32 script;
    u8 padf48[8];
    s32 peerIndex;
    s32 number;
    u8 padf58;
    u8 message;
    u8 oldNumber;
    u8 delay;
    u8 connection;
};
s32 LinkPanel_GetConnectionStatus(struct LinkPanel *);
s32 Registry1745_GetSessionState(void);
void ScrollingDialogue_ClearScript(void);
s32 Script_ParseStringRef(u32);
const u8 *Text_LookupString(s32);
void ScrollingDialogue_SetScript(const u8 *);
void ScrollingDialogue_SetNumberArgument(s32, s32);
static inline u8 connected(void)
{
    if ((*(u8 **)&gLinkSession) && (*(u8 **)&gLinkSession)[0x1d])
        return 1;
    return 0;
}
static inline s32 peerStatus(s32 i)
{
    if (!(*(u8 **)&gLinkSession))
        return 0;
    {
        u8 *statuses = (*(u8 **)&gLinkSession) + 0x64;
        return statuses[i];
    }
}
static inline u8 delayMessage(struct LinkPanel *p)
{
    if (p->delay <= 30) {
        p->delay++;
        return 1;
    }
    return 0;
}
void LinkPanel_UpdateStatusMessage(struct LinkPanel *p)
{
    s32 state, status, connection;
    connection = LinkPanel_GetConnectionStatus(p);
    if (connection < 0 || p->connection != connection) {
        p->delay = 0;
        p->connection = connection;
    }
    if (p->peerIndex < 0) {
        if ((s8)p->message == 2)
            return;
        if (delayMessage(p))
            return;
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->script) + 2));
        p->message = 2;
        return;
    }
    state = Registry1745_GetSessionState();
    if ((u32)(state - 5) <= 1) {
        if ((s8)p->message == 4)
            return;
        if (delayMessage(p))
            return;
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->script) + 4));
        p->message = 4;
        return;
    }
    if (connected() && state == 8) {
        if ((s8)p->message == 6)
            return;
        if (delayMessage(p))
            return;
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->script) + 6));
        p->message = 6;
        return;
    }
    status = peerStatus(p->peerIndex);
    switch (status) {
        case 0:
        case 1:
            if ((s8)p->message == 1)
                return;
            if (delayMessage(p))
                return;
            ScrollingDialogue_ClearScript();
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->script) + 1));
            p->message = 1;
            break;
        case 2:
            if ((s8)p->message == 2)
                return;
            if (delayMessage(p))
                return;
            ScrollingDialogue_ClearScript();
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->script) + 2));
            p->message = 2;
            break;
        case 3:
            if (p->number != p->oldNumber) {
                ScrollingDialogue_ClearScript();
                ScrollingDialogue_SetNumberArgument(0, p->number);
                ScrollingDialogue_SetScript(
                    Text_LookupString(Script_ParseStringRef(p->script) + 3));
                p->message = status;
            } else {
                if ((s8)p->message == 3)
                    return;
                if (delayMessage(p))
                    return;
                ScrollingDialogue_ClearScript();
                ScrollingDialogue_SetNumberArgument(0, p->number);
                ScrollingDialogue_SetScript(
                    Text_LookupString(Script_ParseStringRef(p->script) + 3));
                p->message = 3;
            }
            break;
    }
}
