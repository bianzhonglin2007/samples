

#include <utils/Thread.h>
#include <utils/Mutex.h>
#include <utils/Log.h>
#include <stdio.h>


#undef LOG_TAG
#define LOG_TAG "TEST"
using namespace android;

Mutex mLock;
int count = 0;

//Thread类是纯虚函数，必须要子类集成它， 再实现threadLoop方法。
class Test1 : public Thread
{
public:

private:
	virtual bool threadLoop();

};

bool Test1::threadLoop(){
	AutoMutex _l(::mLock);	
	printf(" i'm test1 threadLoop! count = %d\n", count++);
	sleep(1);
	return true;
}

class Test2 : public Thread
{
public:

private:
	virtual bool threadLoop();

};

bool Test2::threadLoop(){
	AutoMutex _l(::mLock);	
	printf(" i'm test2 threadLoop! count = %d \n", count++);
	sleep(1);
	return true;
}


int main(int argc, char *argv[])
{
	printf(" test \n");
	
	sp<Test1> test1 = new Test1();
	test1->run("test1");


	sp<Test2> test2 = new Test2();
	test2->run("test1");

	while(1) sleep(100);

	return 0;
}
