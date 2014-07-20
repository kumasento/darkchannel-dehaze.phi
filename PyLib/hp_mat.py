#!/usr/local/bin/python

import cv2
import numpy as np

def description():
	print 'hello, this is hp_mat module'

def mat_gen(img_fl_n, mat_fl_n):
	img_cv_f = cv2.imread(img_fl_n)
	img_sp = img_cv_f.shape
	[img_h,img_w] = img_sp[0:2]
	
	mat_fl = open(mat_fl_n, "w")
	print >> mat_fl, "%d %d" % (img_h, img_w)

	[r_mat, g_mat, b_mat] = [[],[],[]]
	for x in range(0, img_h):
		[r_arr, g_arr, b_arr] = [[],[],[]]
		for y in range(0, img_w):
			r_arr.append(str(img_cv_f.item(x,y,2)))
			g_arr.append(str(img_cv_f.item(x,y,1)))
			b_arr.append(str(img_cv_f.item(x,y,0)))
			
		#This append build sth like:['xxx','yyy',...]
		r_mat.append(' '.join(r_arr))
		g_mat.append(' '.join(g_arr))
		b_mat.append(' '.join(b_arr))

	for ln_idx in range(0, img_h):
		print >>mat_fl, r_mat[ln_idx]
	for ln_idx in range(0, img_h):
		print >>mat_fl, g_mat[ln_idx]
	for ln_idx in range(0, img_h):
		print >>mat_fl, b_mat[ln_idx]
	
def img_gen(mat_fl_n, img_fl_n):
	mat_fl = open(mat_fl_n, "r")
	ln_cnt = 0

	h, w = [0, 0]
	r_arr = []
	g_arr = []
	b_arr = []
	for ln in mat_fl:
		ln = ln.split(' ')
		if ln_cnt == 0:
			h, w = [int(ln[0]), int(ln[1])]
		elif ln_cnt == 1:
			r_arr = ln
		elif ln_cnt == 2:
			g_arr = ln
		elif ln_cnt == 3:
			b_arr = ln
		ln_cnt += 1
	
	o_img = np.zeros((h, w, 3), np.uint8)
	for x in range(0, h):
		for y in range(0, w):
			idx = x * w + y
			o_img[x, y] = (np.uint8(b_arr[idx]), np.uint8(g_arr[idx]), np.uint8(r_arr[idx]))
	
	cv2.imwrite(img_fl_n, o_img)

