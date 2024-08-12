#include "ST2DEditor.h"

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
