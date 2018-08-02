#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include<stdlib.h>
#include<string.h>
#include<execinfo.h>


jmp_buf buf;

void handler(int signum)
{
	signal(signum, SIG_DFL);
	longjmp(buf, 1);
}

void printstacktrace( )
{
	const int len=1024;
	void *func[len];
	size_t size;
	int i;
	char **funs;

	size=backtrace(func, len);
	funs=(char**)backtrace_symbols(func, size);
	fprintf(stderr,"System error, Stack trace:\n");
	for(i=0; i < size; ++i) 
		fprintf(stderr,"%d %s \n", i, funs[i]);

	free(funs);
	return ;
}

int main()
{
	printf("SIGSEGV testing !\n");

	signal(SIGSEGV, handler);

	if(!setjmp(buf)) {//try
		unsigned char * p = 0;
		*p = 10;
	}else{  //catch
		printf(" catch a SIGSEGV error! \n");
		printstacktrace();
	}
	
	printf("SIGSEGV, but continue go on!\n");

	return 0;
}
