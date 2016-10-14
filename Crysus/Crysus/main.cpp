#include <Windows.h>
#include <iostream>

#include "Network.h"

int main()
{
	printf("Crysus::Network::IsInternetAvailable(void) : %s\n",
		Crysus::Network::IsInternetAvailable() ? "true" : "false");
	printf("Crysus::Network::UpdateAvailable(void) : %s\n",
		Crysus::Network::UpdateAvailable(10299) ? "true" : "false");

	system("pause");

	return 0;
}