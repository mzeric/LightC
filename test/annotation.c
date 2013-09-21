//#include "../common.h"

__attribute__((annotate("foo")))
int main(){

	__attribute__((annotate("sky"))) int test;
	volatile int test2 = 3;

	return test2 + 1;
}
