include ../configure.txt

OBJS_LIB = printf.obj

# デフォルト動作

default :
	$(MAKE) stdlib.lib

# ファイル生成規則

stdlib.lib : Makefile $(OBJS_LIB)
	$(GOLIB) $(OBJS_LIB) out:$@

# 一般規則

%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# コマンド

clean :
	-$(DEL) *.lst
	-$(DEL) *.obj

src_only :
	$(MAKE) clean
	-$(DEL) nnlib.lib
