#include "global.h"
#include "libm_compat.h"
double sub_08229E28(double);
double sub_08229FEC(double);
double sub_08229560(double);
double sub_0824B314(double);
double sub_0824B25C(double);
s32 sub_0824A0D8(double,double);
double sub_08249A4C(double,double);
double sub_082499E4(double,double);
double sub_08249A14(double,double);
double sub_08249CF4(double,double);
struct SA230 { double d[12]; s32 n[4]; };
void sub_0822A230(struct SA230 *s,s32 mode,double arg,s32 day,double longitude,double latitude)
{
 double l,b,e,decl,alpha,hour;

 l=sub_08229E28(arg);
 b=sub_08229FEC(arg);
 e=sub_08229560(arg);
 b=b/57.29577951308238;
 l=l/57.29577951308238;
 e=e/57.29577951308238;
 decl=sub_0824B314(sub_082499E4(sub_08249A4C(sub_08249A4C(cos(b),sin(l)),sin(e)),sub_08249A4C(sin(b),cos(e))));
 {double x,y;
 x=cos(b)*cos(l);
 y=sub_08249A14(sub_08249A4C(sub_08249A4C(cos(b),sin(l)),cos(e)),sub_08249A4C(sin(b),sin(e)));
 alpha=atan(y/x);
 if(x<0.0) alpha+=3.14159265358979;
 if(alpha<0.0) alpha+=6.28318530717958;
 {double lat=latitude*0.017453292519943278;
 hour=sub_0824B25C(sub_082499E4(-tan(lat)*tan(decl),sub_08249CF4(cos(1.5856316254368468),cos(lat)*cos(decl))));
 {double rise,transit,date;
 rise=(alpha-hour)/6.28318530717958;
 s->d[3]=rise;
 transit=alpha/6.28318530717958;
 s->d[4]=transit;
 s->d[5]=(alpha+hour)/6.28318530717958;
 date=day;
 if(mode==1) {
  if(date+rise<s->d[9] && s->n[1]==0) {
   date+=1.0;
   s->n[0]++;
  } else if(date+s->d[3]>s->d[10] && s->n[0]==0) {
   date-=1.0;
   s->n[1]++;
  }
 } else if(mode==2) {
  double t=date+transit;
  if(t<s->d[9]) date+=1.0;
  else if(t>s->d[10]) date-=1.0;
 } else if(mode==3) {
  if(s->d[5]>1.0) s->d[5]-=1.0;
  if(date+s->d[5]<s->d[9] && s->n[3]==0) {
   date+=1.0;
   s->n[2]++;
  } else if(date+s->d[5]>s->d[10] && s->n[2]==0) {
   date-=1.0;
   s->n[3]++;
  }
 }
 x=(date+s->d[3])-0.671262;
 {double offset;
 offset=longitude/360.0;
 s->d[0]=(x-offset)/1.002737909+40000.0;
 s->d[1]=((date+s->d[4]-0.671262)-offset)/1.002737909+40000.0;
 s->d[2]=((date+s->d[5]-0.671262)-offset)/1.002737909+40000.0;
 }
 }
 }
 }
}
