#include "SFMLTestbed.h"

int main()
{
	ST::Log::init();

	{
		STEditor::SFMLTestbedEditor testbed;
		testbed.exec();
	}

	ST::Log::destroy();
	return 0;
}
