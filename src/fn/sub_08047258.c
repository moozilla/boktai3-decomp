#include "damage_number_display.h"
s32 DamageNumberDisplay_Initialize(struct DamageNumberDisplay *);
s32 DamageNumberDisplay_Update(struct DamageNumberDisplay *);
u32 DamageNumberDisplay_Destroy(struct DamageNumberDisplay *);
struct DamageNumberDisplay *DamageNumberDisplay_Create(void)
{
    struct DamageNumberDisplay *display;
    if (gDamageNumberDisplay)
        return gDamageNumberDisplay;
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
