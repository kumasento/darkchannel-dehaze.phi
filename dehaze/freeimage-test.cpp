#include <iostream>
#include "FreeImage.h"

using namespace std;

int main(){
	FreeImage_Initialise();
	cout << "FreeImage_" << FreeImage_GetVersion() << endl;
	cout << FreeImage_GetCopyrightMessage() << endl;
	FreeImage_DeInitialise();
	return 0;
}
