#include "global.h"
#include "engine.h"
#include "ram.h"

struct SelectionPanel {
    u8 unk0[0x420];
    u16 entryIds[10];
};

void SelectionPanel_InitEntryIds(struct SelectionPanel *p)
{
    p->entryIds[0] = 0x1e;
    p->entryIds[2] = 0x2f;
    p->entryIds[3] = 0x1d;
    p->entryIds[4] = 1;
    p->entryIds[5] = 0x21;
    p->entryIds[6] = 0x26;
    {
        u32 mask = 0x800;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            p->entryIds[1] = 0x20;
        else
            p->entryIds[1] = 0x1f;
    }
    p->entryIds[7] = 0x41;
    p->entryIds[8] = 0x42;
    p->entryIds[9] = 0x7f;
}
