#include "global.h"
#include "runtime.h"
struct S {
    u8 pad[9];
    u8 f9;
    u8 fa;
    u8 fb;
};
void GameState_ClearSessionFlags(void)
{
    if (((struct S *)gSessionState)) {
        ((struct S *)gSessionState)->f9 = 0;
        ((struct S *)gSessionState)->fb = 0;
        ((struct S *)gSessionState)->fa = 0;
    }
}
