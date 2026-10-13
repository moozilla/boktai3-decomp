// MATCHING_CONCESSION: register-pinning
#include "global.h"
void *sub_08224EBC(int, int, int);
void SceneMessages_Enqueue(void *, void *);
void sub_0821AD08(int, int);
void sub_08020D68(void *, int);
void SceneMessages_SwapBuffers(void *);
void SceneMessages_Dispatch(void *);
int SceneMessages_Update(u8 *p)
{
    int dispatch = 0;
    int active = p[0x23];
    if (active == 1) {
        p[0x1f] = 0;
        if (p[0x1e]) {
            {
                int next = *(u16 *)(p + 0x1a) + 1;
                register int zero asm("r1") = 0;
                *(u16 *)(p + 0x1a) = next;
                *(u16 *)(p + 0x1c) = 0;
                p[0x1e] = zero;
                p[0x1f] = active;
            }
        }
        if (p[0x1f]) {
            void *message;
            *(u16 *)(p + 0x1c) = 0;
            while ((message =
                        sub_08224EBC(*(s16 *)(p + 0x18), *(s16 *)(p + 0x1a), *(u16 *)(p + 0x1c)))) {
                SceneMessages_Enqueue(p, message);
                (*(u16 *)(p + 0x1c))++;
            }
            if (!*(u16 *)(p + 0x1c)) {
                if (*(u16 *)(p + 0x20))
                    sub_0821AD08(*(u16 *)(p + 0x20), 0);
                p[0x23] = 0;
                return 0;
            }
            dispatch = 1;
        }
        if (*(int *)(p + 0x68)) {
            if (--*(int *)(p + 0x68) == 0)
                sub_08020D68(p + 0x78, 1);
        }
    }
    SceneMessages_SwapBuffers(p);
    if (dispatch)
        SceneMessages_Dispatch(p);
    return 0;
}
