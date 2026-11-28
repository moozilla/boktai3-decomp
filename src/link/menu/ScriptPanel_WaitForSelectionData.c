#include "global.h"
#include "link/link_script_panel.h"

u8 LinkSession_ConsumeStateEntry(struct LinkScriptPanel *);
void LinkSession_SetState(struct LinkScriptPanel *, u32, void *);
s32 Serial_ReadLatestPacket(u16 *);
void LinkMenu_WaitForReadyPeers(void);
void ScriptPanel_PollSelection(void);
void Serial_StopCommunication(void);
s32 ScriptPanel_WaitForSelectionData(struct LinkScriptPanel *p)
{
    u16 packets[4];
    s32 result;
    s32 i;

    if (LinkSession_ConsumeStateEntry(p))
        p->retryTimer = 0;
    result = Serial_ReadLatestPacket(packets);
    if (p->playerSlot >= 0 && result >= 0 && packets[p->playerSlot] != 0xFFFF) {
        LinkSession_SetState(p, 4, LinkMenu_WaitForReadyPeers);
    } else if (++p->retryTimer > 29) {
        LinkSession_SetState(p, 2, ScriptPanel_PollSelection);
        Serial_StopCommunication();
        for (i = 0; i < 2; i++)
            p->buttons[i] = 0;
    }
    return 0;
}
