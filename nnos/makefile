include ../configure.txt

OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj asminterrupt.obj interrupt.obj \
				memory.obj buffer.obj rtc.obj sheet.obj parallel.obj

# デフォルト動作

default :
	$(MAKE) ipl.bin
	$(MAKE) nnos.sys

# ファイル生成規則

ipl.bin : ipl.nas Makefile
	$(NASK) $< $@ ipl.lst

asmhead.bin : asmhead.nas makefile
	$(NASK) $< $@ asmhead.lst

hankaku.bin : hankaku.fnt makefile
	$(MAKEFONT) $< $@

hankaku.obj : hankaku.bin makefile
	$(BIN2OBJ) $< $@ _hankaku

bootpack.bim : $(OBJS_BOOTPACK) makefile
	$(OBJ2BIM) @$(RULEFILE) out:$@ stack:3136k map:bootpack.map $(OBJS_BOOTPACK)

bootpack.hrb : bootpack.bim makefile
	$(BIM2HRB) $< $@ 0

nnos.sys : asmhead.bin bootpack.hrb makefile
ifeq ($(TARGET),linux)
	cat $< bootpack.hrb > $@
else
	copy /B $<+bootpack.hrb $@
endif

# 一般規則

%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# コマンド

clean :
	-$(DEL) asmhead.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.hrb

src_only :
	$(MAKE) clean
	-$(DEL) ipl.bin
	-$(DEL) nnos.sys
