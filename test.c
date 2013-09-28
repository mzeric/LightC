/*#include <stdio.h>*/
int global;
int _tmain(int a ,int b,int first){
	const int var ;
//	int ;
	volatile int c = 3;
	char p = "hello";
/*	for(; c< 10; c++){



	}
*/
	var = c;
	c = 100+b;

	var = var + 3;
	var = a + c + var;

//	return var;

}

int f(char b){
	int i = 3;
	i = i + 1;
}

