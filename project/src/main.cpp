//External includes
#ifdef ENABLE_VLD
#include "vld.h"
#endif
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "Timer.h"
#include "Renderer.h"
#include "Scene.h"

using namespace dae;

void PrintInfo();

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"RayTracer - Mauro Deryckere",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);

	//auto const pScene { new Scene_W1() };
	//auto const pScene{ new Scene_W2() };
	//auto const pScene{ new Scene_W3() };
	//auto const pScene{ new Scene_W3_TestScene() };

	//auto const pScene{ new Scene_TriangleTest{} };
	//auto const pScene{ new Scene_W4_TestScene{} };

	auto const pScene{ new Scene_W4_ReferenceScene{} };
	//auto const pScene{ new Scene_W4_BunnyScene{} };

	//auto const pScene{ new Scene_Softshadows{} };

	PrintInfo();

	pScene->Initialize();

	//Start loop
	pTimer->Start();

	// Start Benchmark
	// pTimer->StartBenchmark();

	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;

				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->ToggleShadows();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F3)
				{
					pRenderer->CycleLighMode();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					pRenderer->CycleSampleMode();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					pRenderer->DecreaseSamples();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F6)
				{
					pRenderer->IncreaseSamples();
				}

				break;
			}
		}

		//--------- Update ---------
		pScene->Update(pTimer);
		//--------- Render ---------
		pRenderer->Render(pScene);

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pScene;
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}

void PrintInfo()
{
	std::cout << "Raytracer project Mauro Deryckere\n";
	std::cout << "Keybinds: \n";
	std::cout << "F1: Screenshot\nF2: Shadows on/off\nF3: Cycle light mode\nF4: Cycle sample mode\nF5: Decrease samples\nF6: Increase samples\n\n";
	std::cout << "WASD: Move camera\nHold LMB and move: rotate camera\n\n";
}