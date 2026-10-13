#include "message_menu.h"
int MessageMenu_Initialize(struct MessageMenu *m)
{
    struct MenuResource *r = ScriptProc_Start(0xCB05, 0xE2AB);
    if (!r)
        return -1;
    m->resource = *r;
    ResourceHeader_Relocate(&m->resource, r);
    m->timer = 0;
    if (!Script_SeekToKeyword('c'))
        return -1;
    m->source = Text_LookupString(Script_ParseStringRef(Script_GetPc()));
    if (!Script_SeekToKeyword('d'))
        return -1;
    m->text = sub_08227E90();
    if (!m->text)
        return -1;
    m->script = Script_GetKeywordValue('e', 0);
    sub_08044E6C(m->formatter);
    sub_08044DD0(m->lines, m->formatter, m->source, 6);
    MessageMenu_LoadBackground(m, 12);
    if (MessageMenu_CreateTextWindows(m) != 0)
        return -1;
    MessageMenu_InitializeTransition(m);
    return 0;
}
