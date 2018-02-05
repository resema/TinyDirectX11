#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstane, HINSTANE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	// Create the system object
	System = new SystemClass;
	if (!System) return 0;

	// initialze and run the system oject
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// shutdown and release the system objet
	System->Shutdown();
	delete Syste;
	System = 0;

	return 0;
}