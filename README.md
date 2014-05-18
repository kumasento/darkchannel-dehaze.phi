DEHAZE-CECA
===========

###2014-05-14:

This is the initial version of our dehaze project, please follow the instructions to make sure the code could work.  
You can do it either on your machine or the server. The differences will be listed below.

1. unzip the FreeImage in the Packages dir, and then go into the dir to compile the code. You can find the tutorial of compilation. Please remember to use sudo.
2. try `make freeimage-test` and run `bin/freeimage-test`, if you found there's something wrong, follow the step 3.
3. run this in the command line: `export LD_LIBRARY_PATH="/usr/lib":$(LD_LIBRARY_PATH)`

Good luck
    
###2014-05-17：
The dark channel generator has been added, you could access to the dark channel value by function: `pixelsGetDarkChannelValueByCoord(x, y)`  
The `patch_size` parameter could be assigned in commandline: `... -w [patch_size]`
If you want to get the gray-scale picture of the dark channel, pls use the function:`pixelsSaveImageDarkChannelBitmap()`, and the file name would be `dark_channel_bitmap_[patch_size].png`

Good luck

Vincent
