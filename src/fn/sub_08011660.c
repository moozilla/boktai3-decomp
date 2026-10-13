#include "password.h"
int NameEntry_DecodeAndValidate(struct PasswordEntry *m)
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
