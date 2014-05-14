freeimage-test:
	g++ -Wall -I./FreeImageLib/ -L./FreeImageLib/ -lfreeimage -lfreeimage-3.15.4 freeimage-test.cpp -o ./bin/freeimage-test

freeimage-test-makefile-test:
	g++ -Wall -L./ -lfreeimage freeimage-test.cpp -o ./bin/freeimage-test
