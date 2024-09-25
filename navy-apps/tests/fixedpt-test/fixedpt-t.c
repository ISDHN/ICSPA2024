#include <stdio.h>
#include <fixedptc.h>

int main(void) {
	fixedpt a = fixedpt_rconst(1.5);
	fixedpt b = fixedpt_rconst(2.5);
	fixedpt na = fixedpt_rconst(-1.5);
	fixedpt nb = fixedpt_rconst(-2.5);

	fixedpt c = fixedpt_add(a, b);
	fixedpt d = fixedpt_sub(a, b);
	fixedpt f = fixedpt_mul(a, b);
	fixedpt g = fixedpt_div(a, b);

	fixedpt h = fixedpt_ceil(na);
	fixedpt i = fixedpt_floor(na);
	fixedpt j = fixedpt_abs(na);

	fixedpt k = fixedpt_ceil(a);
	fixedpt l = fixedpt_floor(a);
	fixedpt m = fixedpt_abs(a);
	printf("%f %f %f %f\n", fixedpt_tofloat(c), fixedpt_tofloat(d), fixedpt_tofloat(f), fixedpt_tofloat(g));
	printf("%f %f %f\n", fixedpt_tofloat(h), fixedpt_tofloat(i), fixedpt_tofloat(j));
	printf("%f %f %f\n", fixedpt_tofloat(k), fixedpt_tofloat(l), fixedpt_tofloat(m));
	return 0;
}