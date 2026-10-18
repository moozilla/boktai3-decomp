#include "global.h"
struct PanelSequence {
    u8 pad0[0x20];
    u8 sprites[7][0x60];
    u8 pad2C0[0x20];
    u8 resource[0x20];
    u16 resourceIds[7];
    u16 previousIds[7];
    void *script;
};
void *ScriptProc_Start(u16, u16);
void ResourceHeader_Relocate(void *, void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
void PanelSequence_InitializeSprites(struct PanelSequence *group)
{
    u8 *sprite;
    int i;
    void *script;
    int zero;
    script = ScriptProc_Start(0xCB05, 0x1289);
    group->script = script;
    ResourceHeader_Relocate(group->resource, script);
    i = 0;
    zero = 0;
    sprite = (u8 *)group + 0x20;
    do {
        sub_0821983C(sprite, group->resource, group->resourceIds[i], 0x10, zero, zero, zero, (void *)zero);
        group->previousIds[i] = 0xFFFF;
        sprite += 0x60;
        i++;
    } while (i < 7);
}
