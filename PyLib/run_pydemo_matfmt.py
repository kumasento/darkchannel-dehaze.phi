import ctypes

pyMatFmt = ctypes.cdll.LoadLibrary('pyMatFmt.so')

in_fl_n = ctypes.c_char_p('tmp_mat_file')
out_fl_n = ctypes.c_char_p('out_tmp_mat_file')

pyMatFmt.pyMatFmtLnW(in_fl_n, out_fl_n)
