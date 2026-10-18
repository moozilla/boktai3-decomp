#include "global.h"
struct SceneMenuDisplayElement {
    u8 bytes[0x60];
};
struct SceneMenuDisplay {
    u8 pad0[0x20];
    struct SceneMenuDisplayElement elements[7];
    u8 auxiliaryResource[0x20];
    u8 mainResource[0x20];
    u16 animations[7];
    u16 selected[6];
    u16 auxiliarySelection;
    void *script;
};
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void sub_0821983C(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneMenu_InitializeDisplay(struct SceneMenuDisplay *p)
{
    s32 i;
    struct SceneMenuDisplayElement *element;
    void *resource;
    s32 zero;
    resource = ScriptProc_Start(0xCB05, 0x1289);
    p->script = resource;
    ResourceHeader_Relocate(p->mainResource, resource);
    resource = ScriptProc_Start(0xCB05, 0x530D);
    ResourceHeader_Relocate(p->auxiliaryResource, resource);
    i = 0;
    zero = 0;
    element = p->elements;
    do {
        sub_0821983C(element, p->mainResource, p->animations[i], 0x10, 1, zero, zero, (void *)zero);
        p->selected[i] = -1;
        element++;
        i++;
    } while (i <= 5);
    {
        u8 clear = 0;
        p->elements[2].bytes[0x1A] = clear;
        p->elements[3].bytes[0x1A] = clear;
        sub_0821983C(&p->elements[6], p->auxiliaryResource, p->animations[6], 0x10, clear, clear,
                     clear, (void *)clear);
        p->auxiliarySelection |= -1;
    }
}
