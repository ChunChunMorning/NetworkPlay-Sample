# include <Siv3D.hpp>

void Main()
{
	Rect leftBar(50, 180, 30, 120);
	Rect rightBar(560, 180, 30, 120);

	while (System::Update())
	{
		leftBar.draw();
		rightBar.draw();
	}
}
