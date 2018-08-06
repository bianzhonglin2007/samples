
#include <iostream>
#include <string>
#include <memory>

class report {

private:
	std::string str;

public:
	report(const std::string s) : str(s){
		std::cout << "Object create. \n";
	}
	~report(){
		std::cout << "Object delete. \n";
	}
	void comment() const{
		std::cout << str << "\n";
	}
};

int main(int argc, char *argv[])
{
	{
		std::auto_ptr<report> ps(new report("using auto ptr"));
		ps->comment();
	}

	{
		std::shared_ptr<report> ps(new report("using shared ptr"));
		ps->comment();
	}

	{
		std::unique_ptr<report> ps(new report("using unique ptr"));
		ps->comment();
	}

	return 0;
}
