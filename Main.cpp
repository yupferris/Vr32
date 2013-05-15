#include "Common.h"
#include "VirtualBoy.h"

int Main(const List<String>& arguments)
{
	try
	{
		VirtualBoy virtualBoy;
		String romFileName, ramFileName;

		bool running = true;

		auto window = Window::Create("Vr32", virtualBoy.GetOutputWidth(), virtualBoy.GetOutputHeight());
		window->Closing += [&] ()
		{
			running = false;
		};
		window->KeyDown += [&] (Key key)
		{
			if (key == Key::Escape) running = false;
		};

		auto loadRomFile = [&] (const String& fileName)
		{
			romFileName = fileName;
			if (romFileName.Length())
			{
				try
				{
					if (!romFileName.EndsWith(".vb")) throw FSL_EXCEPTION("ROM files must have a .vb extension");
					virtualBoy.LoadRom(File::ReadAllBytes(romFileName));
					ramFileName = romFileName.Substring(romFileName.Length() - 2) + "ram";
					try
					{
						virtualBoy.LoadRam(File::ReadAllBytes(ramFileName));
					}
					catch (...) { }
				}
				catch (const Exception& e)
				{
					MessageWindow::Error(window, e.GetMsg());
					romFileName = ramFileName = "";
				}
			}
		};

		auto menu = Menu::Create();
		auto fileMenu = Menu::Create("File");
		auto fileOpenRom = MenuItem::Create("Load ROM...");
		fileOpenRom->Click += [&] { loadRomFile(DialogWindow::OpenFile(window, "Load ROM").ToLower()); };
		fileMenu->AddChild(fileOpenRom);
		fileMenu->AddSeparator();
		auto fileExit = MenuItem::Create("Exit");
		fileExit->Click += [&] () { running = false; };
		fileMenu->AddChild(fileExit);
		menu->AddChild(fileMenu);
		auto systemMenu = Menu::Create("System");
		auto systemReset = MenuItem::Create("Reset");
		systemReset->Click += [&] () { virtualBoy.Reset(); };
		systemMenu->AddChild(systemReset);
		menu->AddChild(systemMenu);
		auto helpMenu = Menu::Create("Help");
		auto helpAbout = MenuItem::Create("About...");
		helpAbout->Click += [&] () { MessageWindow::Info(window, "Vr32 - A Virtual Boy emulator"); };
		helpMenu->AddChild(helpAbout);
		menu->AddChild(helpMenu);
		window->SetMenu(menu);

		auto viewport = Viewport::Create();
		window->SetContent(viewport);

		if (arguments.Count()) loadRomFile(arguments[0]); // TODO: proper args

		while (running)
		{
			virtualBoy.Update();
			window->Update();
		}

		if (ramFileName.Length() && virtualBoy.GetRamSize())
		{
			try
			{
				File::WriteBytes(ramFileName, virtualBoy.GetRam(), virtualBoy.GetRamSize());
			}
			catch (const Exception& e)
			{
				MessageWindow::Error(e.GetMsg());
			}
		}

		delete window;
		delete menu;
		delete fileMenu;
		delete fileOpenRom;
		delete fileExit;
		delete systemMenu;
		delete systemReset;
		delete helpMenu;
		delete helpAbout;
		delete viewport;
	}
	catch (const Exception& e)
	{
		MessageWindow::Error(e.GetMsg());
		return 1;
	}

	return 0;
}
