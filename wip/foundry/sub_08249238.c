#define Callback_Invoke sub_08249238
#include "global.h"

/* ABI trampoline: the ROM branches directly to the callback in r0. */
void sub_08249238(void (*callback)(void))
{
    callback();
}
