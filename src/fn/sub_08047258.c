#include "damage_number_display.h"
#define DamageNumberDisplay_Create     sub_08047258
#define DamageNumberDisplay_Initialize sub_080471CC
#define DamageNumberDisplay_Update     sub_08047168
#define DamageNumberDisplay_Destroy    sub_080471A8
s32 DamageNumberDisplay_Initialize(struct DamageNumberDisplay *);
s32 DamageNumberDisplay_Update(struct DamageNumberDisplay *);
u32 DamageNumberDisplay_Destroy(struct DamageNumberDisplay *);
struct DamageNumberDisplay *DamageNumberDisplay_Create(void)
{
    struct DamageNumberDisplay *display;
    if (gUnk_02000074)
        return gUnk_02000074;
    display = Task_Create(9, sizeof(*display));
    if (display) {
        Task_SetCallbacks(display, (void (*)(void *))DamageNumberDisplay_Update,
                          (void (*)(void *))DamageNumberDisplay_Destroy);
        if (DamageNumberDisplay_Initialize(display) < 0) {
            Task_Kill(display);
            return NULL;
        }
    }
    return display;
}
