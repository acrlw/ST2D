#include "ST2DEditor.h"
#pragma comment(linker, "/STACK:104857600")

int main()
{
	ST::Log::init();

	{
		STEditor::ST2DEditor testbed;
		testbed.exec();
	}

	ST::Log::destroy();
	return 0;
}
