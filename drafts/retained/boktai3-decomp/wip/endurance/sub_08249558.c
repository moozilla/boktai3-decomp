// CFLAGS: -O2
typedef float SFtype __attribute__ ((mode (SF)));
typedef float DFtype __attribute__ ((mode (DF)));
typedef int HItype __attribute__ ((mode (HI)));
typedef int SItype __attribute__ ((mode (SI)));
typedef int DItype __attribute__ ((mode (DI)));
typedef unsigned int UHItype __attribute__ ((mode (HI)));
typedef unsigned int USItype __attribute__ ((mode (SI)));
typedef unsigned int UDItype __attribute__ ((mode (DI)));
 typedef UDItype fractype;
 typedef USItype halffractype;
 typedef DFtype FLO_type;
 typedef DItype intfrac;
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
  halffractype words[2];
  struct
    {
      fractype fraction:52 __attribute__ ((packed));
      unsigned int exp:11 __attribute__ ((packed));
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
extern FLO_type sub_08249558 ( fp_number_type * );
FLO_type
sub_08249558 ( fp_number_type * src)
{
  FLO_union_type dst;
  fractype fraction = src->fraction.ll;
  int sign = src->sign;
  int exp = 0;
  if (isnan (src))
    {
      exp = (0x7ff);
      if (src->class == CLASS_QNAN || 1)
 {
   fraction |= 0x8000000000000LL;
 }
    }
  else if (isinf (src))
    {
      exp = (0x7ff);
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
      if (src->normal_exp < (-(1023)+1))
 {
   int shift = (-(1023)+1) - src->normal_exp;
   exp = 0;
   if (shift > 64 - 8L)
     {
       fraction = 0;
     }
   else
     {
       fraction >>= shift;
     }
   fraction >>= 8L;
 }
      else if (src->normal_exp > 1023)
 {
   exp = (0x7ff);
   fraction = 0;
 }
      else
 {
   exp = src->normal_exp + 1023;
   if ((fraction & 0xff) == 0x80)
     {
       if (fraction & (1 << 8L))
  fraction += 0x7f + 1;
     }
   else
     {
       fraction += 0x7f;
     }
   if (fraction >= (1LL<<(52 +1+8L)))
     {
       fraction >>= 1;
       exp += 1;
     }
   fraction >>= 8L;
 }
    }
  dst.bits.fraction = fraction;
  dst.bits.exp = exp;
  dst.bits.sign = sign;
  {
    halffractype tmp = dst.words[0];
    dst.words[0] = dst.words[1];
    dst.words[1] = tmp;
  }
  return dst.value;
}
extern void sub_082496A0 (FLO_union_type *, fp_number_type *);
int __fpcmp_parts_d (fp_number_type * a, fp_number_type *b);
extern SFtype sub_0824ABFC (fp_class_type, unsigned int, int, USItype);
