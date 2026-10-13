// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Tag {
    u8 pad[0x30];
    int hasValue, length;
    u8 text[64];
};
const u8 *sub_08032254(struct Tag *p, const u8 *s)
{
    int n;
    int first;
    u8 *text;
    p->hasValue = 0;
    p->length = 0;
    n = 0;
    first = *s;
    if (first == '>')
        goto done;
    text = p->text;
    if (!first)
        goto done;
    do {
        int length;
        int c = *s;
        if (c == '=') {
            register int one asm("r0") = 1;
            p->hasValue = one;
        } else if (p->hasValue) {
            length = p->length;
            text[length] = c;
            p->length = length + 1;
        }
        n++;
        s++;
        if (n > 63)
            break;
        first = *s;
        if (first == '>')
            break;
    } while (first);
done:
    p->text[p->length] = 0;
    return s;
}
