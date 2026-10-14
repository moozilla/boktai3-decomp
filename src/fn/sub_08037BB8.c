#include "global.h"
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
void *sub_08227E90(void);
void *Script_GetKeywordValue(int, int);
void sub_08037228(struct P *);
int sub_08037290(struct P *, struct Panel *, int);
int sub_08033690(int, int, int, int);
void TextWindow_SetScript(int, void *);
void sub_08033A38(int, int, void *);
void TextWindow_Refresh(int, int);
void sub_080337FC(int);
void WirelessLinkIndicator_SetPosition(int, int);
void sub_082279A8(int, int, int, int, int, int, int);
void sub_0821983C(void *, void *, int, int, int, int, int, int);
int sub_08037BB8(struct P *p)
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
    p->script = sub_08227E90();
    if (!p->script)
        goto fail;
    p->title = Script_GetKeywordValue(0x72, 0);
    p->state = 0;
    p->entered = 1;
    p->timer = 0;
    sub_08037228(p);
    q = p->panels;
    i = 0;
    do {
        if (sub_08037290(p, q, i) < 0)
            goto fail;
        i++;
        q++;
    } while (i <= 1);
    w = &p->windows[0];
    *w = sub_08033690(3, 10, 10, 2);
    if (*w < 0)
        goto fail;
    TextWindow_SetScript(*w, p->script);
    sub_08033A38(*w, 0, (u8 *)p + 0x25c);
    TextWindow_Refresh(*w, 3);
    sub_080337FC(*w);
    w = &p->windows[1];
    *w = sub_08033690(17, 10, 10, 2);
    if (*w < 0)
        goto fail;
    TextWindow_SetScript(*w, p->script);
    sub_08033A38(*w, 1, (u8 *)p + 0x4b8);
    TextWindow_Refresh(*w, 4);
    sub_080337FC(*w);
    w = &p->windows[2];
    *w = sub_08033690(0, 14, 30, 6);
    if (*w < 0)
        goto fail;
    TextWindow_SetScript(*w, p->script);
    TextWindow_Refresh(*w, 7);
    sub_080337FC(*w);
    WirelessLinkIndicator_SetPosition(0, 24);
    sub_082279A8(2, 4, 4, 4, 4, 65535, 0);
    b = ScriptProc_Start(0xcb05, 0x7c8b);
    if (!b)
        goto fail;
    p->blob2 = *b;
    ResourceHeader_Relocate(&p->blob2, b);
    sub_0821983C(p->draw, &p->blob2, 1, 16, 0, 0, 60, 0);
    return 0;
fail:
    return -1;
}
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
int sub_08037B20(struct P *);
int sub_08037B6C(struct P *);
struct P *sub_08037D80(void)
{
    struct P *p = Task_Create(11, 0x694);
    if (p) {
        Task_SetCallbacks(p, sub_08037B20, sub_08037B6C);
        if (sub_08037BB8(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
extern struct P *gUnk_020000FC;
struct P *sub_08037DC0(void)
{
    return gUnk_020000FC;
}
void *sub_08037DCC(int i)
{
    struct P *p = sub_08037DC0();
    void *r;
    if (p)
        r = (u8 *)p + 0xf4 + i * 16;
    else
        r = 0;
    return r;
}
