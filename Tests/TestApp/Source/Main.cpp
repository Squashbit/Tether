#include <iostream>

#include <SBSTD/Stopwatch.hpp>

#include <Link/Link.hpp>

#include <math.h>

using namespace Link;

class TestWindow : public Link::Window
{
public:
	class EventHandler : public Link::Events::EventHandler
	{
	public:
		EventHandler(TestWindow* pWindow)
			:
			pWindow(pWindow)
		{}
	private:
		TestWindow* pWindow = nullptr;
	};

	TestWindow()
		:
		handler(this)
	{
	}

	~TestWindow()
	{
		for (uint64_t i = 0; i < shits.size(); i++)
			delete shits[i];
	}

	void InitializeComponent()
	{
		SetBackgroundColor(Color(0.1f, 0.1f, 0.1f));
		
		uint64_t numShits = 1920;
		for (uint64_t i = 0; i < numShits; i++)
		{
			uint64_t height = i * i;
			height /= 1000;
			
			Link::Controls::Panel* shit = new Link::Controls::Panel();
			shit->SetX(i);
			shit->SetWidth(1);
			shit->SetY(1000 - height);
			shit->SetHeight(height);
			shit->SetForegroundColor(Color(1, 1, 1));

			shits.push_back(shit);

			AddControlNoRepaint(shit);
		}
		
		Repaint();
		
		Show();
	}

	std::vector<Link::Controls::Panel*> shits;
private:
	EventHandler handler;
};

int main()
{
	TestWindow window;
	if (window.Init() != ErrorCode::SUCCESS)
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}
	
	window.SetTitle("yep");

	while (!window.IsCloseRequested())
	{
		window.PollEvents();
	}

	return 0;
}