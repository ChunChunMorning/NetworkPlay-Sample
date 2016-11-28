
# include <Siv3D.hpp>

int GetInput()
{
	int move = 0;

	if (Input::KeyUp.pressed)
		move -= 1;

	if (Input::KeyDown.pressed)
		move += 1;

	return move;
}

void Main()
{
	Rect leftBar(50, 180, 30, 120);
	Rect rightBar(560, 180, 30, 120);

	while (System::Update())
	{
		leftBar.y += 3 * GetInput();
		rightBar.y += 3 * GetInput();

		leftBar.draw();
		rightBar.draw();
	}
}
