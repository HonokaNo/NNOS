include ../configure.txt

OBJS_API = putchar.obj putstr0.obj putstr1.obj end.obj openwin.obj putstrwin.obj \
			boxfilwin.obj initmalloc.obj malloc.obj free.obj point.obj refreshwin.obj \
			linewin.obj closewin.obj getkey.obj alloctimer.obj inittimer.obj settime.obj \
			freetimer.obj gettimer.obj beep.obj fopen.obj fclose.obj fseek.obj fsize.obj fread.obj \
			cmdline.obj getlang.obj getclock.obj getwindata.obj exec.obj fwrite.obj \
			alloca.obj

# デフォルト動作

default :
	$(MAKE) nnlib.lib

# ファイル生成規則

nnlib.lib : Makefile $(OBJS_API)
	$(GOLIB) $(OBJS_API) out:$@

# 一般規則

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# コマンド

clean :
	-$(DEL) *.lst
	-$(DEL) *.obj

src_only :
	$(MAKE) clean
	-$(DEL) nnlib.lib
