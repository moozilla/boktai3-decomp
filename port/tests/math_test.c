double __adddf3(double,double); double __subdf3(double,double); double __muldf3(double,double); double __divdf3(double,double); double __floatsidf(int); int __fixdfsi(double); double __kernel_sin(double,double,int); double __ieee754_sqrt(double);
int math_test_run(void) {
 if(__adddf3(1.5,2.25)!=3.75) return 1;
 if(__subdf3(1.5,2.25)!=-0.75) return 2;
 if(__muldf3(-2.0,4.0)!=-8.0) return 3;
 if(__divdf3(7.0,2.0)!=3.5) return 4;
 if(__floatsidf(-123456)!=-123456.0) return 5;
 if(__fixdfsi(-3.75)!=-3) return 6;
 if(__ieee754_sqrt(9.0)!=3.0) return 7;
 { double x=__kernel_sin(0.5,0.0,0); if(x<0.4794255386042029 || x>0.4794255386042031) return 8; }
 return 0;
}
#if !defined(__wasm__)
int main(void) { return math_test_run(); }
#endif
