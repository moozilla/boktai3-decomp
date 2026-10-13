#include "global.h"
struct Resource {
    u32 words[8];
};
struct Menu {
    u8 task[24];
    u32 timer;
    u8 pad[6];
    u16 script;
    u8 formatter[24];
    u8 lines[1024];
    void *source, *text;
    int handles[3];
    void *map, *palette;
    struct Resource resource;
    u8 transition[96];
};
void *ScriptProc_Start(int, int);
void ResourceHeader_Relocate(void *, void *);
int Script_SeekToKeyword(int);
int Script_GetPc(void);
int Script_ParseStringRef(int);
void *Text_LookupString(int);
void *sub_08227E90(void);
int Script_GetKeywordValue(int, int);
void sub_08044E6C(void *);
void sub_08044DD0(void *, void *, void *, int);
void MessageMenu_LoadBackground(void *, int);
int MessageMenu_CreateTextWindows(void *);
void MessageMenu_InitializeTransition(void *);
int MessageMenu_Initialize(struct Menu *m)
{
    struct Resource *r = ScriptProc_Start(0xCB05, 0xE2AB);
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
