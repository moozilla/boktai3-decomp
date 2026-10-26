#include "global.h"
#include "background_controller.h"
/* Boot_ClearReservedWords: Clears four reserved EWRAM boot words right after AgbMain; standalone 28-byte function with its own push/pop-bx, matching the documented reserved-word */
#define Boot_ClearReservedWords sub_08001590
#define Boot_ClearReservedWords sub_08001590
// Provisional name: Boot_ClearReservedWords
/* Clear the four boot-state words at the start of external RAM. */
void Boot_ClearReservedWords(void)
{
    u32 *start = gBackgrounds;
    u32 zero = 0;
    u32 *p = start + 3;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)start);
}
