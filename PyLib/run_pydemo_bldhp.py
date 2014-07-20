import ctypes

pyMatFmt = ctypes.cdll.LoadLibrary('pyBldHp.so')

in_fl_n = ctypes.c_char_p('tmp_mat_file')

pyMatFmt.pyBldHpFromMatFmt(in_fl_n)
