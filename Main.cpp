
# include <Siv3D.hpp>
# include <HamFramework.hpp>
# include "asc/TCPString.hpp"

struct GameData
{
	asc::TCPStringClient client;
};

class Connecting : public SceneBase<String, GameData>
{
private:

	Font font;
	String buffer;

public:

	void init() override
	{
		font = Font(30);
		m_data->client.connect(IPv4(127, 0, 0, 1), 50000);
	}

	void update() override
	{
		if (m_data->client.hasError())
		{
			m_data->client.disconnect();
			m_data->client.connect(IPv4(127, 0, 0, 1), 50000);
		}

		if (m_data->client.readLine(buffer))
		{
			if (buffer == L"welcome\n")
			{
				changeScene(L"waiting", 0);
			}
		}
	}

	void draw() const override
	{
		font(L"接続待機中...").drawCenter(Window::Height() / 2);
	}
};

class Waiting : public SceneBase<String, GameData>
{
private:

	Font font;
	Optional<int64> time;
	String buffer;

public:

	void init() override
	{
		font = Font(30);
		time = none;
	}

	void update() override
	{
		if (m_data->client.hasError())
		{
			m_data->client.disconnect();
			changeScene(L"connecting", 0);
		}

		if (!time.has_value() && m_data->client.readLine(buffer))
		{
			buffer.pop_back();
			const auto args = buffer.split(L',');

			if (args[0] == L"start")
			{
				time = Parse<int64>(args[1]);
			}
		}

		if (time.has_value() && time <= Time::SecSince1970())
		{
			changeScene(L"game");
		}
	}

	void draw() const override
	{
		font(L"対戦相手を待っています...").drawCenter(Window::Height() / 2);
	}
};

class Game : public SceneBase<String, GameData>
{
private:

	bool isParent;
	bool isSendGoal;
	Rect leftBar;
	int leftBarSpeed;
	Rect rightBar;
	int rightBarSpeed;
	Circle ball;
	Vec2 ballSpeed;
	String buffer;

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
		isParent = false;
		isSendGoal = false;
		leftBar = Rect(50, 180, 30, 120);
		rightBar = Rect(560, 180, 30, 120);
		ball = Circle(320, 240, 10);
		ballSpeed = Vec2(-1, 1);
	}

	void update() override
	{
		if (m_data->client.hasError())
		{
			changeScene(L"connecting");
			return;
		}

		m_data->client.sendString(Format(L"move,", getInput(), L'\n'));

		while (m_data->client.readLine(buffer))
		{
			buffer.pop_back();
			const auto args = buffer.split(L',');

			if (args[0] == L"isParent")
			{
				isParent = true;
			}
			else if (args[0] == L"parent")
			{
				leftBarSpeed = Parse<int>(args[1]);
			}
			else if (args[0] == L"child")
			{
				rightBarSpeed = Parse<int>(args[1]);
			}
			else if (args[0] == L"goal")
			{
				ball.center = Window::Center();
				isSendGoal = false;
			}
		}

		ball.center += 2.0 * ballSpeed;
		leftBar.y += 3 * leftBarSpeed;
		rightBar.y += 3 * rightBarSpeed;

		if (ball.center.y < ball.r || ball.center.y > Window::Height() - ball.r)
		{
			ballSpeed.y = -ballSpeed.y;
		}

		if (ball.intersects(leftBar) || ball.intersects(rightBar))
		{
			ballSpeed.x = -ballSpeed.x;
		}

		if (!isSendGoal && isParent && (ball.center.x < -ball.r || Window::Width() + ball.r < ball.center.x))
		{
			m_data->client.sendString(Format(L"goal\n"));
			isSendGoal = true;
		}
	}

	void draw() const override
	{
		ball.draw(Palette::Yellow);
		leftBar.draw(isParent ? Palette::Red : Palette::White);
		rightBar.draw(isParent ? Palette::White : Palette::Red);
	}
};

void Main()
{
	SceneManager<String, GameData> manager;
	manager.add<Connecting>(L"connecting");
	manager.add<Waiting>(L"waiting");
	manager.add<Game>(L"game");

	while (System::Update())
	{
		manager.updateAndDraw();
	}
}
