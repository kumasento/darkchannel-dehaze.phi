CC = g++ -Wall -O3 -fmessage-length=0

FIVERSION = 3.15.4
INCLUDE = -I./FreeImageLib/
LIBS = -L./FreeImageLib/ -lfreeimage -lfreeimage-$(FIVERSION)

freeimage-test:
	g++ -Wall -I./FreeImageLib/ -L./FreeImageLib/ -lfreeimage -lfreeimage-3.15.4 freeimage-test.cpp -o ./bin/freeimage-test.o

freeimage-test-makefile-test:
	g++ -Wall -L./ -lfreeimage freeimage-test.cpp -o ./bin/freeimage-test.o

freeimage-complete-test:
	$(CC) $(INCLUDE) $(LIBS) freeimage-complete-demo.cpp -o ./bin/freeimage-complete-demo.o

