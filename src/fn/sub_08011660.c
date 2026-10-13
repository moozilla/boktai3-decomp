#include "global.h"
struct Entry {
    u8 codes[24], text[48], decoded[44], name[5], pad[3];
    void *table;
    int script, version, checksum, key;
};
int sub_08044CF4(void *, int *, int, int);
int sub_08221594(void *, int, int);
void sub_080453DC(void *, void *, int);
int sub_080115DC(void *);
#define NameEntry_DecodeAndValidate sub_08011660
int NameEntry_DecodeAndValidate(struct Entry *m)
{
    u8 *decoded = m->decoded;
    int pos = 0;
    m->checksum = sub_08044CF4(m, &pos, 16, 6);
    sub_08044CF4(m, &pos, 2, 6);
    m->version = sub_08044CF4(m, &pos, 3, 6);
    m->key = sub_08044CF4(m, &pos, 3, 6);
    if ((sub_08221594(m, 3, 24) & 65535) != m->checksum)
        return -1;
    sub_080453DC(decoded, m, m->key);
    if (m->version != 1)
        return -2;
    if (!m->name[0] || sub_080115DC(m))
        return -3;
    return 0;
}
