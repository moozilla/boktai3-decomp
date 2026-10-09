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
extern void sub_0824A3AC (FLO_union_type *, fp_number_type *);
void
sub_0824A3AC (FLO_union_type * src, fp_number_type * dst)
{
  fractype fraction;
  int exp;
  int sign;
  fraction = src->bits.fraction;
  exp = src->bits.exp;
  sign = src->bits.sign;
  dst->sign = sign;
  if (exp == 0)
    {
      if (fraction == 0)
 {
   dst->class = CLASS_ZERO;
 }
      else
 {
   dst->normal_exp = exp - 127 + 1;
   fraction <<= 7L;
   dst->class = CLASS_NUMBER;
   while (fraction < (1LL<<(23 +7L)))
     {
       fraction <<= 1;
       dst->normal_exp--;
     }
   dst->fraction.ll = fraction;
 }
    }
  else if (exp == (0xff))
    {
      if (fraction == 0)
 {
   dst->class = CLASS_INFINITY;
 }
      else
 {
   if (fraction & 0x100000L)
     {
       dst->class = CLASS_QNAN;
     }
   else
     {
       dst->class = CLASS_SNAN;
     }
   dst->fraction.ll = fraction;
 }
    }
  else
    {
      dst->normal_exp = exp - 127;
      dst->class = CLASS_NUMBER;
      dst->fraction.ll = (fraction << 7L) | (1LL<<(23 +7L));
    }
}
int __fpcmp_parts_f (fp_number_type * a, fp_number_type *b);
extern DFtype sub_0824A288 (fp_class_type, unsigned int, int, UDItype frac);
