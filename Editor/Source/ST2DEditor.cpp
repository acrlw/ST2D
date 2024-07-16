#include "ST2DEditor.h"

using namespace ST;

namespace STEditor
{
	ST2DEditor::ST2DEditor()
	{

	}

	ST2DEditor::~ST2DEditor()
	{
	}

	void ST2DEditor::exec()
	{
		APP_INFO("Hello World!")
		Vector2 v1(1.0f, 2.0f);
		Matrix2x2 mat(1.0f, 2.0f, 3.0f, 4.0f);
		mat.invert();
		Vector2 col1 = mat.column1;
		Vector2 col2 = mat.column2;
		col1 = 2.0f * col2;
		std::string f = std::format("Matrix is: {}, {}, {}, {}", mat.e11(), mat.e12(), mat.e21(), mat.e22());
		APP_INFO("Matrix is: {}, {}, {}, {}", mat.e11(), mat.e12(), mat.e21(), mat.e22());

		ST::Rectangle rectangle;
		rectangle.set(1.0f, 1.0f);

		ST::ShapePrimitive sp1;
		sp1.shape = &rectangle;
		sp1.transform.position.set(0.0f, 0.0f);
		sp1.userData.uuid = 1;


		ST::ShapePrimitive sp2;
		sp2.shape = &rectangle;
		sp2.transform.position.set(0.0f, 4.0f);
		sp2.userData.uuid = 2;

		auto info = Narrowphase::gjkDistance(sp1, sp2);
		APP_INFO("Distance: {}", info.penetration);
	}
}
