#include <iostream>
#include <string>
#include <memory>

using namespace std;

/*
 把auto_ptr全部替换成shared_ptr就可以运行正常 
 : % s/auto_ptr/shared_ptr 
 */

int main(int argc, char *argv[])
{
	auto_ptr<string> films[5] = 
	{
		auto_ptr<string> (new string("Fowl Balls")),
		auto_ptr<string> (new string("Duck Walks")),
		auto_ptr<string> (new string("Chicken Runs")),
		auto_ptr<string> (new string("Trukey Errors")),
		auto_ptr<string> (new string("Goose Eggs"))
	};


	auto_ptr<string> pwin;
	pwin = films[2]; //films[2] loses ownership, 

	cout << "The nominees for best avian baseball film are \n";
	
	for(int i = 0; i < 5; ++i)
		cout << *films[i] << endl;
	
	cout << "The winner is " << *pwin  << endl;

	cin.get();


	return 0;
}
