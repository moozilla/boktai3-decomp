#include "global.h"
int ResultTutorial_FadeOut(void *);
typedef struct {
    u32 pad[2], flags;
} Graphic;
int ResultTutorial_HidePrompt(u8 *p)
{
    u8 *state = p + 0xaf;
    switch (*state) {
        case 0: {
            int one = 1;
            *state = one;
            *(u16 *)(p + 0xaa) = 0;
            ((Graphic *)(p + 0x684))->flags |= one;
        }
        case 1:
            if (ResultTutorial_FadeOut(p))
                return 1;
            break;
    }
    return 0;
}
