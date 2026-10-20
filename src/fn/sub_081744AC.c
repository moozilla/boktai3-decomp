#include "global.h"

struct PresentationGraphicRequest {
    u8 index;
    u8 unk1;
    u16 flag2;
    u8 flag4;
    u8 unk5[3];
    u8 flag8;
};

struct PresentationGraphicRow {
    u8 stringId;
    u8 unk1;
    u8 numberA;
    u8 numberB;
    u8 unk4[8];
    u8 stringB;
    u8 unkD[3];
    u8 stringC;
    u8 unk11[15];
};

extern const struct PresentationGraphicRow gUnk_08E6092C[];
s32 Script_ParseStringRef(const void *);
void ScrollingDialogue_SetNumberArgument(u32, u32);
void ScrollingDialogue_SetScript(const void *);
void ScrollingDialogue_SetStringArgument(u32, const void *);
const void *Text_LookupString(const void *);

void Presentation_ApplyGraphicSettings(struct PresentationGraphicRequest *req, const void *script)
{
    const struct PresentationGraphicRow *row = &gUnk_08E6092C[req->index];
    u8 *base = (u8 *)Script_ParseStringRef(script);
    u8 *strings = base + 0x31;
    s32 i;
    if (row->numberA)
        ScrollingDialogue_SetNumberArgument(0, row->numberA);
    ScrollingDialogue_SetNumberArgument(1, row->numberB);
    ScrollingDialogue_SetScript(Text_LookupString(strings + row->stringId));
    i = 0;
    if (req->flag4)
        ScrollingDialogue_SetStringArgument(0, Text_LookupString(strings + (row->stringB + 0x31)));
    else
        ScrollingDialogue_SetStringArgument(0, Text_LookupString(base + 0x62));
    i++;
    if (req->flag8)
        ScrollingDialogue_SetStringArgument(i, Text_LookupString(strings + (row->stringC + 0x31)));
    else
        ScrollingDialogue_SetStringArgument(i, Text_LookupString(strings + 0x31));
    i++;
    if (req->flag2 == 0) {
        ScrollingDialogue_SetStringArgument(i, Text_LookupString(strings + 0x49));
        i++;
    }
    while (i <= 3) {
        ScrollingDialogue_SetStringArgument(i, Text_LookupString(strings + 0x31));
        i++;
    }
}
