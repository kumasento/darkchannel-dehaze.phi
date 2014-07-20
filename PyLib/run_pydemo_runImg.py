import ctypes
import os
import getopt, sys

import hp_mat

def usage():
	print "... -h[help] -f [file_name] -p [patch_name] -o [out_img_name]"

def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'hf:p:o:')
	except getopt.GetoptError as err:
		print str(err)
		usage()
		sys.exit(2)

	img_f_n = None
	ptch = 3
	out_f_n = 'py.png'

	for o, a in opts:
		if o == '-h':
			usage()
			sys.exit()
		elif o == '-f':
			img_f_n = a
		elif o == '-p':
			ptch = int(a)
		elif o == '-o':
			out_f_n = a

	print "Parameter Parser's Result:"
	print "Image File Name: %s" % img_f_n
	print "Running Patch Size: %d" % ptch
	print "Output Image Name: %s" % out_f_n
	
	in_fl = 'tmp_mat_file'
	out_fl = 'out_tmp_mat_file'
	hp_mat.mat_gen(img_f_n, in_fl)

	pyMatFmt = ctypes.cdll.LoadLibrary('PyLib/pyRunImg.so')
	in_fl_c = ctypes.c_char_p(in_fl)
	out_fl_c = ctypes.c_char_p(out_fl)
	ptch_c = ctypes.c_int(ptch)
	
	print "\nRunning C++ Dehaze Kernel...\n"
	pyMatFmt.pyRunImgDehaze(in_fl_c, ptch_c, out_fl_c)
	print "Done\n"

	print "Saving..."
	hp_mat.img_gen(out_fl, out_f_n) 
	print "Done"
	os.system('rm %s' % in_fl)
	os.system('rm %s' % out_fl)

if __name__ == "__main__":
	main()
