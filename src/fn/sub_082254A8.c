#include "global.h"
struct S {
    u8 pad[9];
    u8 f9;
    u8 fa;
    u8 fb;
};
extern struct S *gSessionState;
void GameState_ClearSessionFlags(void)
{
    if (gSessionState) {
        gSessionState->f9 = 0;
        gSessionState->fb = 0;
        gSessionState->fa = 0;
    }
}
