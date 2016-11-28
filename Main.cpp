
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
	Circle ball(320, 240, 10);
	Vec2 ballSpeed = Vec2(-1, 1);

	while (System::Update())
	{
		ball.center += 3 * ballSpeed;
		leftBar.y += 3 * GetInput();
		rightBar.y += 3 * GetInput();

		if (ball.center.y < ball.r || ball.center.y > Window::Height() - ball.r)
			ballSpeed.y = -ballSpeed.y;

		if (ball.intersects(leftBar) || ball.intersects(rightBar))
			ballSpeed.x = -ballSpeed.x;

		ball.draw(Palette::Yellow);
		leftBar.draw();
		rightBar.draw();
	}
}
