#include "global.h"
struct S {
    u8 a[0x96];
    u16 h96;
    u32 w98;
};
void Script_RunBlock(u32, u32);
void Script_Call(u32, u32);
void Camera_RunCompletionScript(struct S *p)
{
    u32 w = p->w98;
    if (w != 0) {
        if (p->h96 == 0) {
            p->w98 = 0;
            Script_RunBlock(w, 0);
        } else {
            p->w98 = 0;
            Script_Call(w, 0);
        }
    }
}
