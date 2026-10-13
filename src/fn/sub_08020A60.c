// MATCHING_CONCESSION: register-pinning
#include "global.h"
int SceneMessages_FindScript(int);
void sub_0821AD08(int, void *);
void sub_08020D68(void *, int);
void SceneMessages_RunScript(void *p, void *entry, u8 *message)
{
    int args[8];
    struct {
        u32 count : 16;
        u32 unused : 16;
        void *args;
    } call;
    int id = (u16)SceneMessages_FindScript(*(u16 *)(message + 8));
    if (id) {
        register int count asm("r0") = message[7];
        {
            register void *cp asm("r5") = &call;
        }
        if (count > 1) {
            s16 *in = (s16 *)(message + 0xa);
            int *out = args;
            int remaining = count - 1;
            asm("" : : "r"(out));
            do {
                *out++ = *in++;
                remaining--;
            } while (remaining);
        }
        {
            u32 mask = 0xffff0000;
            u32 header;
            header = *(u32 *)&call;
            header &= mask;
            header |= 8;
            *(u32 *)&call = header;
        }
        call.args = args;
        sub_0821AD08(id, &call);
    }
    sub_08020D68(entry, 1);
}
