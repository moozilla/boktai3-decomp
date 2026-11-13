/* Portable implementations of the public AGB BIOS operations. No BIOS bytes. */
#include "hw.h"
#include "gba/types.h"
#include <limits.h>

static uint32_t load32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static void store16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

/* Arithmetic right shift with defined behavior on every C implementation. */
static int32_t asr(int32_t v, unsigned n)
{
    if (v >= 0) return (int32_t)((uint32_t)v >> n);
    return (int32_t)~((uint32_t)~v >> n);
}

static int32_t mul32(int32_t a, int32_t b)
{
    return (int32_t)((uint32_t)a * (uint32_t)b);
}

int32_t hw_bios_div(int32_t n, int32_t d, int32_t *remainder)
{
    int32_t q;
    if (!d) {
        q = n < 0 ? 1 : -1;
        if (remainder) *remainder = n;
        return q;
    }
    if (n == INT32_MIN && d == -1) {
        if (remainder) *remainder = 0;
        return INT32_MIN;
    }
    q = n / d;
    if (remainder) *remainder = n % d;
    return q;
}

uint16_t hw_bios_sqrt(uint32_t n)
{
    uint32_t root = 0, bit = 1u << 30;
    while (bit > n) bit >>= 2;
    while (bit) {
        if (n >= root + bit) {
            n -= root + bit;
            root = (root >> 1) + bit;
        } else root >>= 1;
        bit >>= 2;
    }
    return (uint16_t)root;
}

int16_t hw_bios_arctan(int16_t tangent)
{
    /* BIOS's fixed-point odd polynomial: input 1.14, output one turn = 65536. */
    int32_t x = tangent;
    int32_t a = -asr(x * x, 14);
    int32_t b = asr(0xa9 * a, 14) + 0x390;
    b = asr(mul32(b, a), 14) + 0x91c;
    b = asr(mul32(b, a), 14) + 0xfb6;
    b = asr(mul32(b, a), 14) + 0x16aa;
    b = asr(mul32(b, a), 14) + 0x2081;
    b = asr(mul32(b, a), 14) + 0x3651;
    b = asr(mul32(b, a), 14) + 0xa2f9;
    return (int16_t)asr(mul32(x, b), 16);
}

uint16_t hw_bios_arctan2(int16_t x, int16_t y)
{
    int32_t ax = x < 0 ? -(int32_t)x : x;
    int32_t ay = y < 0 ? -(int32_t)y : y;
    int32_t a;
    if (!y) return x < 0 ? 0x8000 : 0;
    if (!x) return y < 0 ? 0xc000 : 0x4000;
    if (ax >= ay) {
        a = hw_bios_arctan((int16_t)(((int32_t)y * 16384) / x));
        if (x < 0) a += 0x8000;
    } else {
        a = 0x4000 - hw_bios_arctan((int16_t)(((int32_t)x * 16384) / y));
        if (y < 0) a += 0x8000;
    }
    return (uint16_t)a;
}

int32_t Div(int32_t n, int32_t d) { return hw_bios_div(n, d, NULL); }
int32_t Mod(int32_t n, int32_t d) { int32_t r; hw_bios_div(n, d, &r); return r; }
int32_t DivArmMod(int32_t d, int32_t n) { return Mod(n, d); }
uint16_t Sqrt(uint32_t n) { return hw_bios_sqrt(n); }
int16_t ArcTan(int16_t x) { return hw_bios_arctan(x); }
uint16_t ArcTan2(int16_t x, int16_t y) { return hw_bios_arctan2(x, y); }

void CpuSet(const void *source, void *destination, uint32_t control)
{
    const uint8_t *src = source;
    uint8_t *dst = destination;
    uint32_t count = control & 0x1fffff;
    unsigned width = (control & 0x04000000) ? 4 : 2;
    src = (const uint8_t *)((uintptr_t)src & ~(uintptr_t)(width - 1));
    dst = (uint8_t *)((uintptr_t)dst & ~(uintptr_t)(width - 1));
    while (count--) {
        uint8_t word[4];
        unsigned i;
        for (i = 0; i < width; ++i) word[i] = src[i];
        for (i = 0; i < width; ++i) dst[i] = word[i];
        if (!(control & 0x01000000)) src += width;
        dst += width;
    }
}

void CpuFastSet(const void *source, void *destination, uint32_t control)
{
    const uint8_t *src = (const uint8_t *)((uintptr_t)source & ~(uintptr_t)3);
    uint8_t *dst = (uint8_t *)((uintptr_t)destination & ~(uintptr_t)3);
    uint32_t count = ((control & 0x1fffff) + 7) & ~7u;
    uint8_t block[32];
    unsigned i;
    if (!count) return;
    if (control & 0x01000000)
        for (i = 0; i < sizeof(block); ++i) block[i] = src[i & 3];
    /* Keep the rounded count outside CpuSet's 21-bit mask. Each BIOS transfer
     * loads all eight words before storing them, including overlapping ranges. */
    while (count) {
        if (!(control & 0x01000000)) {
            for (i = 0; i < sizeof(block); ++i) block[i] = src[i];
            src += sizeof(block);
        }
        for (i = 0; i < sizeof(block); ++i) dst[i] = block[i];
        dst += sizeof(block);
        count -= 8;
    }
}

int hw_bios_decompress(unsigned swi, const void *source, size_t src_size,
                       void *destination, size_t dst_size, size_t *written)
{
    const uint8_t *src = source;
    uint8_t *dst = destination;
    size_t in = 4, out = 0, length;
    unsigned type;
    if (written) *written = 0;
    if (!src || src_size < 4) return HW_TRUNCATED;
    length = load32(src) >> 8;
    type = src[0];
    if (length > dst_size) return HW_NO_SPACE;
    if (length && !dst) return HW_NO_SPACE;
    switch (swi) {
    case 0x11: case 0x12:
        if (type != 0x10) return HW_BAD_DATA;
        while (out < length) {
            unsigned flags, bit;
            if (in >= src_size) return HW_TRUNCATED;
            flags = src[in++];
            for (bit = 0x80; bit && out < length; bit >>= 1) {
                if (flags & bit) {
                    unsigned count, distance;
                    if (src_size - in < 2) return HW_TRUNCATED;
                    count = (src[in] >> 4) + 3;
                    distance = ((src[in] & 15) << 8 | src[in + 1]) + 1;
                    in += 2;
                    if (distance > out) return HW_BAD_DATA;
                    while (count-- && out < length) {
                        dst[out] = dst[out - distance];
                        ++out;
                    }
                } else {
                    if (in >= src_size) return HW_TRUNCATED;
                    dst[out++] = src[in++];
                }
            }
        }
        break;
    case 0x14: case 0x15:
        if (type != 0x30) return HW_BAD_DATA;
        while (out < length) {
            unsigned flag, count;
            if (in >= src_size) return HW_TRUNCATED;
            flag = src[in++];
            count = (flag & 127) + ((flag & 128) ? 3 : 1);
            if (flag & 128) {
                uint8_t value;
                if (in >= src_size) return HW_TRUNCATED;
                value = src[in++];
                while (count-- && out < length) dst[out++] = value;
            } else {
                while (count-- && out < length) {
                    if (in >= src_size) return HW_TRUNCATED;
                    dst[out++] = src[in++];
                }
            }
        }
        break;
    case 0x13: {
        size_t tree_end, node = 5;
        uint32_t bits = 0;
        unsigned remaining = 0, nibble = 0;
        if (type != 0x24 && type != 0x28) return HW_BAD_DATA;
        if (in >= src_size) return HW_TRUNCATED;
        tree_end = 4 + ((size_t)src[in] + 1) * 2;
        if (tree_end > src_size) return HW_TRUNCATED;
        in = tree_end;
        while (out < length) {
            unsigned branch, descriptor, leaf;
            size_t child;
            if (!remaining) {
                if (src_size - in < 4) return HW_TRUNCATED;
                bits = load32(src + in);
                in += 4;
                remaining = 32;
            }
            branch = bits >> 31;
            bits <<= 1;
            --remaining;
            if (node >= tree_end) return HW_BAD_DATA;
            descriptor = src[node];
            child = (node & ~(size_t)1) + ((descriptor & 63) + 1) * 2 + branch;
            if (child >= tree_end) return HW_BAD_DATA;
            leaf = descriptor & (branch ? 0x40 : 0x80);
            if (leaf) {
                if (type == 0x28) dst[out++] = src[child];
                else {
                    if (src[child] > 15) return HW_BAD_DATA;
                    if (!nibble) dst[out] = src[child];
                    else dst[out++] |= (uint8_t)(src[child] << 4);
                    nibble ^= 1;
                }
                node = 5;
            } else node = child;
        }
        break;
    }
    case 0x16: case 0x17: case 0x18: {
        uint16_t sum = 0;
        unsigned width = swi == 0x18 ? 2 : 1;
        if (type != (width == 2 ? 0x82 : 0x81) || length % width) return HW_BAD_DATA;
        if (src_size - in < length) return HW_TRUNCATED;
        while (out < length) {
            sum = (uint16_t)(sum + src[in++]);
            if (width == 2) {
                sum = (uint16_t)(sum + ((uint16_t)src[in++] << 8));
                store16(dst + out, sum);
            } else dst[out] = (uint8_t)sum;
            out += width;
        }
        break;
    }
    default: return HW_UNSUPPORTED;
    }
    if (written) *written = out;
    return HW_OK;
}

static void uncompress(unsigned swi, const uint32_t *src, void *dst)
{
    int result = hw_bios_decompress(swi, src, SIZE_MAX, dst, SIZE_MAX, NULL);
    Hw *hw = hw_bound();
    if (result && hw && hw->host.bios_error) hw->host.bios_error(hw->host.user, swi, result);
}

void LZ77UnCompWram(const uint32_t *s, void *d) { uncompress(0x11, s, d); }
void LZ77UnCompVram(const uint32_t *s, void *d) { uncompress(0x12, s, d); }
void RLUnCompWram(const uint32_t *s, void *d) { uncompress(0x14, s, d); }
void RLUnCompVram(const uint32_t *s, void *d) { uncompress(0x15, s, d); }
void HuffUnComp(const uint32_t *s, void *d) { uncompress(0x13, s, d); }
void Diff8bitUnFilterWram(const uint32_t *s, void *d) { uncompress(0x16, s, d); }
void Diff8bitUnFilterVram(const uint32_t *s, void *d) { uncompress(0x17, s, d); }
void Diff16bitUnFilter(const uint32_t *s, void *d) { uncompress(0x18, s, d); }

/* Quarter-wave sine in 2.14, at the BIOS's 256 angular steps. */
static const int16_t sine_quarter[65] = {
    0, 402, 803, 1205, 1605, 2005, 2404, 2801, 3196, 3589, 3980, 4369, 4756, 5139, 5519, 5896, 6269, 6639, 7005, 7366, 7723, 8075, 8423, 8765, 9102, 9434, 9759, 10079, 10393, 10701, 11002, 11297, 11585, 11866, 12139, 12406, 12665, 12916, 13159, 13395, 13622, 13842, 14053, 14255, 14449, 14634, 14810, 14978, 15136, 15286, 15426, 15557, 15678, 15790, 15892, 15985, 16069, 16142, 16206, 16260, 16305, 16339, 16364, 16379, 16384
};

static int32_t sine(unsigned angle)
{
    unsigned quadrant = (angle & 255) >> 6;
    unsigned index = angle & 63;
    int32_t value = sine_quarter[(quadrant & 1) ? 64 - index : index];
    return quadrant >= 2 ? -value : value;
}

static void affine(int16_t sx, int16_t sy, uint16_t alpha, int16_t out[4])
{
    int32_t sn = sine(alpha >> 8), cs = sine((alpha >> 8) + 64);
    out[0] = (int16_t)asr(sx * cs, 14);
    out[1] = (int16_t)-asr(sx * sn, 14);
    out[2] = (int16_t)asr(sy * sn, 14);
    out[3] = (int16_t)asr(sy * cs, 14);
}

void BgAffineSet(struct BgAffineSrcData *src, struct BgAffineDstData *dst, int32_t count)
{
    while (count-- > 0) {
        int16_t m[4];
        affine(src->sx, src->sy, src->alpha, m);
        dst->pa = m[0]; dst->pb = m[1]; dst->pc = m[2]; dst->pd = m[3];
        dst->dx = (int32_t)((uint32_t)src->texX - (uint32_t)(m[0] * src->scrX) - (uint32_t)(m[1] * src->scrY));
        dst->dy = (int32_t)((uint32_t)src->texY - (uint32_t)(m[2] * src->scrX) - (uint32_t)(m[3] * src->scrY));
        ++src; ++dst;
    }
}

void ObjAffineSet(struct ObjAffineSrcData *src, void *destination, int32_t count, int32_t stride)
{
    uint8_t *dst = destination;
    while (count-- > 0) {
        int16_t m[4];
        unsigned i;
        affine(src->xScale, src->yScale, src->rotation, m);
        for (i = 0; i < 4; ++i) {
            store16(dst, (uint16_t)m[i]);
            dst += stride;
        }
        /* The BIOS source record has a reserved halfword after rotation. The
         * reconstructed SDK declaration omits it, so use the wire stride. */
        src = (struct ObjAffineSrcData *)((uint8_t *)src + 8);
    }
}
