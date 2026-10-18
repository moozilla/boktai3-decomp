#define AnimPanel_JoinTextRows sub_081DF1A0
#include "global.h"
struct AnimPanelText {
    u8 rows[4][256];
};
void sub_081DF1A0(struct AnimPanelText *text, u8 *out)
{
    s32 row = 0;
    for (row = 0; row <= 3; row++) {
        u8 *in = text->rows[row];
        s32 count = 0;
        u8 c = *in;
        if (c != 0) {
            do {
                *out = c;
                in++;
                out++;
                count++;
            } while (count <= 255 && (c = *in) != 0);
        }
    }
    *out = 0;
}
