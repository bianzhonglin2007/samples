

#include <utils/RefBase.h>
#include <utils/Log.h>
#include <stdio.h>


#define LOG_TAG "TEST"
using namespace android;

class A : public RefBase
{

public:
	A(){
		//TODO: 设置对象默认生命周期，强引用控制，还是弱引用控制
		extendObjectLifetime(1);
		printf(" create A \n");
	}

	~A(){
		printf(" delete A \n");
	}
};


int main(int argc, char *argv[])
{
	printf("test A create and delete \n");


	//TODO: 如果是普通对象指针，用完需要手动调用delete释放。
	A* pA = new A;

	{
		sp<A> spA(pA);
		wp<A> wpA(spA);
	}

	//TODO: sp<A> spA(pA)的时候pA对象的生命周期就已经由spA指针接管，spA变量销毁的时候pA对象就已经销毁，不能重复销毁, 否则出现segment fault;
	//delete pA; 

	//TODO: 如果直接定义pB对象指针，pB可以自动释放
	//sp<A> pB = new A;

	return 0;
}
