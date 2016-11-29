
# include <Siv3D.hpp>
# include <HamFramework.hpp>

struct GameData
{

};

class Waiting : public SceneBase<String, GameData>
{
private:

	Font font;

public:

	void init() override
	{
		font = Font(30);
	}

	void update() override
	{
		
	}

	void draw() const override
	{
		font(L"接続待機中...").drawCenter(Window::Height() / 2);
	}
};

class Game : public SceneBase<String, GameData>
{
private:

	Rect leftBar;
	Rect rightBar;
	Circle ball;
	Vec2 ballSpeed;

	int getInput()
	{
		int move = 0;

		if (Input::KeyUp.pressed)
			move -= 1;

		if (Input::KeyDown.pressed)
			move += 1;

		return move;
	}

public:

	void init() override
	{
		leftBar = Rect(50, 180, 30, 120);
		rightBar = Rect(560, 180, 30, 120);
		ball = Circle(320, 240, 10);
		ballSpeed = Vec2(-1, 1);
	}

	void update() override
	{
		ball.center += 3 * ballSpeed;
		leftBar.y += 3 * getInput();
		rightBar.y += 3 * getInput();

		if (ball.center.y < ball.r || ball.center.y > Window::Height() - ball.r)
			ballSpeed.y = -ballSpeed.y;

		if (ball.intersects(leftBar) || ball.intersects(rightBar))
			ballSpeed.x = -ballSpeed.x;
	}

	void draw() const override
	{
		ball.draw(Palette::Yellow);
		leftBar.draw();
		rightBar.draw();
	}
};

void Main()
{
	SceneManager<String, GameData> manager;
	manager.add<Waiting>(L"waiting");
	manager.add<Game>(L"game");

	while (System::Update())
	{
		manager.updateAndDraw();
	}
}
