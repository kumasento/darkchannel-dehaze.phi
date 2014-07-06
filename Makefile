CC = g++ -Wall -fmessage-length=0 -g

FIVERSION = 3.15.4
INCLUDE = -I./FreeImageLib/ -I./
LIBS = -L./FreeImageLib/ -lfreeimage -lfreeimage-$(FIVERSION)
CV_INC = -I/usr/local/Cellar/opencv/ -I./

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

dehaze-framework-2a:
	$(CC) $(INCLUDE) $(LIBS) dehaze-framework-2a.cpp -o ./bin/dehaze-framework-2a.o -g

dehaze-demo-v1:
	$(CC) $(CV_INC) dehaze-demo-v1.cpp -o ./bin/dehaze-demo-v1.o

dehaze-dark-channel-gen:
	$(CC) $(INCLUDE) $(LIBS) dehaze-dark-channel-gen.cpp -o ./bin/dehaze-dark-channel-gen.o

general-matrix-prefix:
	$(CC) -I./ GeneralMatrixTester.cpp -o ./bin/general_matrix_tester.o

general-matrix-boxfilter:
	$(CC) -I./ GeneralMatrixBoxFilter.cpp -o ./bin/general_matrix_box_filter.o

general-matrix-guidedfilter:
	$(CC) -I./ GeneralMatrixGuidedFilterTester.cpp -o ./bin/general_matrix_guided_filter.o

