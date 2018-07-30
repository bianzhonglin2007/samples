
#include <iostream>
#include <thread>

using namespace std;

void thread01(int num)
{
	for(int i = 0; i < num; i++)
	{
		cout << "Thread 01 is working!\n " << endl;
		sleep(1);
	}
}

void thread02(int num)
{
	for(int i = 0; i < num; i++)
	{
		cout << "Thread 02 is working!\n " << endl;
		sleep(2);
	}
}


int main(int argc, char *argv[])
{
	thread *task01 = new thread(thread01, 5);
	thread *task02 = new thread(thread02, 5);

	task01->join();
	task02->join();

	for(int i = 0; i < 5; i++){
		cout << "Main thread is working! \n " << endl;
		sleep(1);
	}

	return 0;
}
