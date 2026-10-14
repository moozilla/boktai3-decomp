// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#include "scene_vec.h"
void sub_0821983C(void *, void *, u32, u32, u32, u32, u32, void *);
void sub_08217DB4(void *, void *);
s32 ModifierMenu_InitializeGraphics(struct ModifierMenu *menu)
{
    struct ModifierGraphicsResource *resource;
    struct SceneVec origin;
    u32 zero;
    register u32 attribute asm("r8");
    struct SceneVec *originPointer;
    struct ModifierDisplayObject *object;
    struct ModifierGraphicsResource *graphics;
    register s32 remaining asm("r5");
    resource = ScriptProc_Start(0xcb05, 0x530d);
    menu->graphicsA = *resource;
    ResourceHeader_Relocate(&menu->graphicsA, resource);
    resource = ScriptProc_Start(0xcb05, 0x1289);
    menu->graphicsB = *resource;
    ResourceHeader_Relocate(&menu->graphicsB, resource);
    asm("" : "=m"(zero));
    originPointer = &origin;
    zero = 0;
    {
        register u32 *source asm("r0") = &zero;
        void *destination = originPointer;
        asm("" : "+r"(source), "+r"(destination));
        CpuSet(source, destination, 0x05000002);
    }
    object = menu->displays;
    remaining = 17;
    do {
        sub_0821983C(object, &menu->graphicsB, 0, 0x11, 2, 0, 0x3c, originPointer);
        remaining--;
        object++;
    } while (remaining >= 0);
    object = &menu->displays[5];
    graphics = &menu->graphicsB;
    Anim_LoadKeyframe(object, graphics, 0x12, 0);
    object = &menu->displays[6];
    Anim_LoadKeyframe(object, graphics, 0x10, 0);
    object->attribute1A = 1;
    object = &menu->displays[7];
    Anim_LoadKeyframe(object, graphics, 0x11, 0);
    object->attribute1A = 1;
    object = &menu->displays[13];
    if (menu->specialHeader)
        Anim_LoadKeyframe(object, &menu->graphicsA, 0x80, 0);
    else
        Anim_LoadKeyframe(object, &menu->graphicsA, 0x7f, 0);
    object->y = 8;
    object = &menu->displays[14];
    attribute = 0;
    asm("" : "+r"(attribute));
    remaining = 3;
    do {
        Anim_LoadKeyframe(object, &menu->graphicsA, 0x50, 0);
        object->z = 50;
        object->attribute1A = attribute;
        remaining--;
        object++;
    } while (remaining >= 0);
    sub_08217DB4(menu->palette, gUnk_030042E4 + 0x64e0);
    object = &menu->displays[1];
    object->tile = 0x9010;
    object->palette = menu->palette;
    object->attribute1A = 0;
    object = &menu->displays[8];
    object->tile = 0x9010;
    object->palette = menu->palette;
    object->attribute1A = 0;
    return 0;
}
