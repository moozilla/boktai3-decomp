#include "global.h"
#include "libm_compat.h"
struct SunContext { double d[12]; s32 n[4]; };
double sub_08229468(s32,u32,s32,s32);
double sub_08229FFC(s32,u32,s32,s32);
void sub_0822A230(struct SunContext*,s32,double,s32,double,double);
void sub_08228A10(s32,s32);void sub_08228B10(s32,s32);void sub_08229078(s32,s32);
s32 sub_0824A040(double,double);
s32 Mod(s32,s32);s32 Div(s32,s32);
static inline double transform(double date,double offset)
{
 return ((date-40000.0)*1.002737909+0.671262)+offset;
}
u32 Time_CalculateSunriseSunsetCore(u32 year,s32 month,s32 day,double latitude,double longitude,s32 zone)
{
 struct SunContext c;
 double middle,rise,set,offset;
 offset=longitude/360.0;
 c.d[6]=sub_08229468((s32)&c,year,month,day)-0.375;
 c.d[9]=transform(c.d[6],offset);
 c.d[7]=sub_08229468((s32)&c,year,month,day+1)-0.375;
 c.d[10]=transform(c.d[7],offset);
 {
  double midpoint=c.d[6]+0.5;
  double transit;
  c.d[8]=midpoint;
  c.d[11]=transform(midpoint,offset);
  c.n[0]=0;c.n[1]=0;c.n[2]=0;c.n[3]=0;
  sub_0822A230(&c,1,midpoint,(s32)c.d[11],longitude,latitude);
  transit=c.d[1];
  sub_0822A230(&c,0,transit,(s32)transform(transit,offset),longitude,latitude);
 }
 {
 double old=c.d[0],current;
 middle=c.d[2];
 sub_0822A230(&c,1,old,(s32)transform(old,offset),longitude,latitude);
 current=c.d[0];
 sub_0822A230(&c,1,current,(s32)transform(current,offset),longitude,latitude);
 {
  double result=c.d[0],difference;
  if(current>result) difference=current-result;else difference=result-current;
  {
   s32 comparison=sub_0824A040(difference,0.7);
   rise=result;
   if(comparison>0) rise=-1.0;
  }
 }
 }
 {
  double old;
  double intermediate=(middle-40000.0)*1.002737909+0.671262;
  double offset2=longitude/360.0;
  sub_0822A230(&c,3,middle,(s32)(intermediate+offset2),longitude,latitude);
  old=c.d[2];
  sub_0822A230(&c,3,old,(s32)transform(old,offset2),longitude,latitude);
  {
   double result=c.d[2],difference;
   if(old>result) difference=old-result;else difference=result-old;
   {
    s32 comparison=sub_0824A040(difference,0.7);
    set=result;
    if(comparison>0) set=-1.0;
   }
  }
 }
 {
  double fraction=rise-(double)(s32)rise+0.375;
  if(fraction>=1.0) fraction-=1.0;
  if(rise>=c.d[6] && rise<c.d[6]+1.0) {
   double hours=fraction*24.0;
   s32 hour=(s32)hours;
   double minutes=(hours-(double)hour)*60.0;
   s32 minute=(s32)minutes,rounded;
   if(minutes-(double)minute<0.5) rounded=(s32)minutes;else rounded=minute+1;
   if(rounded==60) {rounded=0;hour++;if(hour==24) {hour=23;rounded=59;}}
   hour+=zone;
   if(hour<0) hour+=24;else if(hour>23) hour-=24;
   sub_08228A10(hour,rounded);
  }
 }
 {
  double fraction=set-(double)(s32)set+0.375;
  if(fraction>=1.0) fraction-=1.0;
  if(set>=c.d[6] && set<c.d[6]+1.0) {
   double hours=fraction*24.0;
   s32 hour=(s32)hours;
   double minutes=(hours-(double)hour)*60.0;
   s32 minute=(s32)minutes,rounded;
   if(minutes-(double)minute<0.5) rounded=(s32)minutes;else rounded=minute+1;
   if(rounded==60) {rounded=0;hour++;if(hour==24) {hour=23;rounded=59;}}
   hour+=zone;
   if(hour<0) hour+=24;else if(hour>23) hour-=24;
   sub_08228B10(hour,rounded);
  }
 }
 {
  s32 value=(s32)(sub_08229FFC((s32)&c,year,month,day)*100.0);
  s32 part=Mod(value,10),whole;
  if(part>4) part=Div(Mod(value,100),10)+1;
  else part=Div(Mod(value,100),10);
  if(part>9) {whole=Div(value,100)+1;part=0;}
  else whole=Div(value,100);
  sub_08229078(whole,part);
 }
 return 0;
}
