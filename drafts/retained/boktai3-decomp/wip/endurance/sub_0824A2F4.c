// CFLAGS: -O2
typedef float SFtype __attribute__ ((mode (SF)));
typedef float DFtype __attribute__ ((mode (DF)));
typedef int HItype __attribute__ ((mode (HI)));
typedef int SItype __attribute__ ((mode (SI)));
typedef int DItype __attribute__ ((mode (DI)));
typedef unsigned int UHItype __attribute__ ((mode (HI)));
typedef unsigned int USItype __attribute__ ((mode (SI)));
typedef unsigned int UDItype __attribute__ ((mode (DI)));
 typedef USItype fractype;
 typedef UHItype halffractype;
 typedef SFtype FLO_type;
 typedef SItype intfrac;
typedef enum
{
  CLASS_SNAN,
  CLASS_QNAN,
  CLASS_ZERO,
  CLASS_NUMBER,
  CLASS_INFINITY
} fp_class_type;
typedef struct
{
  fp_class_type class;
  unsigned int sign;
  int normal_exp;
  union
    {
      fractype ll;
      halffractype l[2];
    } fraction;
} fp_number_type;
typedef union
{
  FLO_type value;
  fractype value_raw;
  struct
    {
      fractype fraction:23 __attribute__ ((packed));
      unsigned int exp:8 __attribute__ ((packed));
      unsigned int sign:1 __attribute__ ((packed));
    }
  bits;
}
FLO_union_type;
__inline__
static fp_number_type *
nan ()
{
  static fp_number_type thenan;
  return &thenan;
}
__inline__
static int
isnan ( fp_number_type * x)
{
  return x->class == CLASS_SNAN || x->class == CLASS_QNAN;
}
__inline__
static int
isinf ( fp_number_type * x)
{
  return x->class == CLASS_INFINITY;
}
__inline__
static int
iszero ( fp_number_type * x)
{
  return x->class == CLASS_ZERO;
}
__inline__
static void
flip_sign ( fp_number_type * x)
{
  x->sign = !x->sign;
}
extern FLO_type sub_0824A2F4 ( fp_number_type * );
FLO_type
sub_0824A2F4 ( fp_number_type * src)
{
  FLO_union_type dst;
  fractype fraction = src->fraction.ll;
  int sign = src->sign;
  int exp = 0;
  if (isnan (src))
    {
      exp = (0xff);
      if (src->class == CLASS_QNAN || 1)
 {
   fraction |= 0x100000L;
 }
    }
  else if (isinf (src))
    {
      exp = (0xff);
      fraction = 0;
    }
  else if (iszero (src))
    {
      exp = 0;
      fraction = 0;
    }
  else if (fraction == 0)
    {
      exp = 0;
    }
  else
    {
      if (src->normal_exp < (-(127)+1))
 {
   int shift = (-(127)+1) - src->normal_exp;
   exp = 0;
   if (shift > 32 - 7L)
     {
       fraction = 0;
     }
   else
     {
       fraction >>= shift;
     }
   fraction >>= 7L;
 }
      else if (src->normal_exp > 127)
 {
   exp = (0xff);
   fraction = 0;
 }
      else
 {
   exp = src->normal_exp + 127;
   if ((fraction & 0x7f) == 0x40)
     {
       if (fraction & (1 << 7L))
  fraction += 0x3f + 1;
     }
   else
     {
       fraction += 0x3f;
     }
   if (fraction >= (1LL<<(23 +1+7L)))
     {
       fraction >>= 1;
       exp += 1;
     }
   fraction >>= 7L;
 }
    }
  dst.bits.fraction = fraction;
  dst.bits.exp = exp;
  dst.bits.sign = sign;
  return dst.value;
}
extern void sub_0824A3AC (FLO_union_type *, fp_number_type *);
int __fpcmp_parts_f (fp_number_type * a, fp_number_type *b);
extern DFtype sub_0824A288 (fp_class_type, unsigned int, int, UDItype frac);
