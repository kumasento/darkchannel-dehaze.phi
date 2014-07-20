CC = g++ -Wall -fmessage-length=0 -g
ICC = icpc -Wall -fmessage-length=0 -mmic -O3

FIVERSION = 3.15.4
CVVERSION = 2.4.9
INCLUDE = -I./FreeImageLib/ -I./
LIBS = -L./FreeImageLib/ -lfreeimage -lfreeimage-$(FIVERSION)
CV_INC = -I/usr/local/Cellar/opencv/$(CVVERSION)/include/ -I./ -I/usr/local/include/
CV_LIB = -L/usr/local/Cellar/opencv/$(CVVERSION)/lib/ -L/usr/local/lib/

filter-1d-test:
	$(CC) filter_1d_test.cpp -o ./bin/filter-1d-test.o

filter-2d-test:
	$(CC) filter_2d_test.cpp -o ./bin/filter-2d-test.o

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

pydemo_runImg:
	$(CC) pydemo_runImg.cpp -I./ -fPIC -shared -o PyLib/pyRunImg.so

pydemo_runImg_mic:
	icpc pydemo_runImg.cpp -I./ -fPIC -shared -o PyLib/pyRunImg.so 

dehaze-framework-1a:
	-DFREE_IMAGE_SUPPORT \
	$(CC) $(INCLUDE) $(LIBS) \ 
	dehaze-framework-1a.cpp -o ./bin/dehaze-framework-1a.o

dehaze-framework-2a:
	-DFREE_IMAGE_SUPPORT
	$(CC) $(INCLUDE) $(LIBS) 
	dehaze-framework-2a.cpp -o ./bin/dehaze-framework-2a.o -g

dehaze-demo-v1:
	$(CC) $(CV_INC) $(CV_LIB) \
	-DOPENCV_SUPPORT \
	-lopencv_core.$(CVVERSION) -lopencv_imgproc.$(CVVERSION) -lopencv_highgui.$(CVVERSION) -lopencv_video.$(CVVERSION) -lopencv_videostab.$(CVVERSION)  \
	dehaze-demo-v1.cpp -o ./bin/dehaze-demo-v1.o 
	
dehaze-demo-v1-linux:
	$(CC) $(CV_INC) $(CV_LIB) \
	-DOPENCV_SUPPORT \
	-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video  \
	dehaze-demo-v1.cpp -o ./bin/dehaze-demo-v1.o 

phi-dehaze-demo-v1:
	$(ICC) $(CV_INC) $(CV_LIB) \
	-DOPENCV_SUPPORT \
	-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video  \
	phi_dehaze_demo_v1.cpp -o ./bin/phi_dehaze_demo_v1.o 

dehaze-dark-channel-gen:
	$(CC) $(INCLUDE) $(LIBS) dehaze-dark-channel-gen.cpp -o ./bin/dehaze-dark-channel-gen.o

general-matrix-prefix:
	$(CC) -I./ GeneralMatrixTester.cpp -o ./bin/general_matrix_tester.o

general-matrix-boxfilter:
	$(CC) -I./ GeneralMatrixBoxFilter.cpp -o ./bin/general_matrix_box_filter.o

general-matrix-guidedfilter:
	$(CC) -I./ GeneralMatrixGuidedFilterTester.cpp -o ./bin/general_matrix_guided_filter.o

