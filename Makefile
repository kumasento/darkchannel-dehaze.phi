CC = g++ -Wall -O3 -fmessage-length=0

FIVERSION = 3.15.4
INCLUDE = -I./FreeImageLib/ -I./
LIBS = -L./FreeImageLib/ -lfreeimage -lfreeimage-$(FIVERSION)

freeimage-test:
	g++ -Wall -I./FreeImageLib/ -L./FreeImageLib/ -lfreeimage -lfreeimage-3.15.4 freeimage-test.cpp -o ./bin/freeimage-test

freeimage-test-makefile-test:
	g++ -Wall -L./ -lfreeimage freeimage-test.cpp -o ./bin/freeimage-test

freeimage-complete-test:
	$(CC) $(INCLUDE) $(LIBS) freeimage-complete-demo.cpp -o ./bin/freeimage-complete-demo

freeimage-load-output-demo:
	$(CC) $(INCLUDE) $(LIBS) freeimage-load-output-demo.cpp -o ./bin/freeimage-load-output-demo.o

freeimage-imageinfo-demo:
	$(CC) $(INCLUDE) $(LIBS) freeimage-imageinfo-demo.cpp -o ./bin/freeimage-imageinfo-demo.o


dehaze-framework-1a:
	$(CC) $(INCLUDE) $(LIBS) dehaze-framework-1a.cpp -o ./bin/dehaze-framework-1a.o

dehaze-dark-channel-gen:
	$(CC) $(INCLUDE) $(LIBS) dehaze-dark-channel-gen.cpp -o ./bin/dehaze-dark-channel-gen.o
