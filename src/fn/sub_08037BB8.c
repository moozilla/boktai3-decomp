#include "global.h"
#include "ram.h"
struct Blob {
    u32 w[8];
};
struct Panel {
    u8 bytes[0x25c];
};
struct P {
    u8 pad[0x19];
    u8 state, entered;
    u8 pad1[5];
    u32 timer;
    u8 pad2[4];
    void *script;
    struct Panel panels[2];
    int windows[3];
    void *title;
    u8 pad3[0x100];
    struct Blob blob, blob2;
    u8 draw[0x60];
};
void *ScriptProc_Start(int, int);
void ResourceHeader_Relocate(void *, void *);
int Script_SeekToKeyword(int);
void *Script_GetTextSourcePc(void);
void *Script_GetKeywordValue(int, int);
void WirelessPeerMenu_InitBackground(struct P *);
int WirelessPeerPanel_Init(struct P *, struct Panel *, int);
int TextWindow_Create(int, int, int, int);
void TextWindow_SetScript(int, void *);
void TextWindow_SetString(int, int, void *);
void TextWindow_Refresh(int, int);
void TextWindow_StartPlayback(int);
void WirelessLinkIndicator_SetPosition(int, int);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void Widget_InitLinked(void *, void *, int, int, int, int, int, int);
int WirelessPeerPanel_Initialize(struct P *p)
{
    struct Blob *b = ScriptProc_Start(0xcb05, 0x530d);
    struct Panel *q;
    int i;
    int *w;
    if (!b)
        goto fail;
    p->blob = *b;
    ResourceHeader_Relocate(&p->blob, b);
    if (!Script_SeekToKeyword(0x73))
        goto fail;
    p->script = Script_GetTextSourcePc();
    if (!p->script)
        goto fail;
    p->title = Script_GetKeywordValue(0x72, 0);
    p->state = 0;
    p->entered = 1;
    p->timer = 0;
    WirelessPeerMenu_InitBackground(p);
    q = p->panels;
    i = 0;
    do {
        if (WirelessPeerPanel_Init(p, q, i) < 0)
            goto fail;
        i++;
        q++;
    } while (i <= 1);
    w = &p->windows[0];
    *w = TextWindow_Create(3, 10, 10, 2);
    if (*w < 0)
        goto fail;
    TextWindow_SetScript(*w, p->script);
    TextWindow_SetString(*w, 0, (u8 *)p + 0x25c);
    TextWindow_Refresh(*w, 3);
    TextWindow_StartPlayback(*w);
    w = &p->windows[1];
    *w = TextWindow_Create(17, 10, 10, 2);
    if (*w < 0)
        goto fail;
    TextWindow_SetScript(*w, p->script);
    TextWindow_SetString(*w, 1, (u8 *)p + 0x4b8);
    TextWindow_Refresh(*w, 4);
    TextWindow_StartPlayback(*w);
    w = &p->windows[2];
    *w = TextWindow_Create(0, 14, 30, 6);
    if (*w < 0)
        goto fail;
    TextWindow_SetScript(*w, p->script);
    TextWindow_Refresh(*w, 7);
    TextWindow_StartPlayback(*w);
    WirelessLinkIndicator_SetPosition(0, 24);
    ScreenTransition_SendCommand(2, 4, 4, 4, 4, 65535, 0);
    b = ScriptProc_Start(0xcb05, 0x7c8b);
    if (!b)
        goto fail;
    p->blob2 = *b;
    ResourceHeader_Relocate(&p->blob2, b);
    Widget_InitLinked(p->draw, &p->blob2, 1, 16, 0, 0, 60, 0);
    return 0;
fail:
    return -1;
}
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
int sub_08037B20(struct P *);
int sub_08037B6C(struct P *);
struct P *WirelessPeerPanel_Create(void)
{
    struct P *p = Task_Create(11, 0x694);
    if (p) {
        Task_SetCallbacks(p, sub_08037B20, sub_08037B6C);
        if (WirelessPeerPanel_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
struct P *WirelessExchange_Get(void)
{
    return gWirelessTradeMenu;
}
void *WirelessExchange_GetPeerEntry(int i)
{
    struct P *p = WirelessExchange_Get();
    void *r;
    if (p)
        r = (u8 *)p + 0xf4 + i * 16;
    else
        r = 0;
    return r;
}
