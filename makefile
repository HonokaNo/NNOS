include configure.txt

# デフォルト動作

default :
	$(MAKE) nnos.img

# ファイル生成規則

nnos.img : nnos/ipl.bin nnos/nnos.sys Makefile \
		winhelo/winhelo.hrb
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:nnos/ipl.bin len:512 from:0 to:0 \
		copy from:nnos/nnos.sys to:@: \
		copy from:nnos/ipl.nas to:@: \
		copy from:nnos/make.bat to:@: \
		copy from:winhelo/winhelo.hrb to:@: \
		copy from:winhelo2/winhelo2.hrb to:@: \
		imgout:nnos.img

# コマンド

run :
	$(MAKE) nnos.img
	$(COPY) nnos.img $(TOOLPATH)qemu/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

full :
	$(MAKE) -C nnos
	$(MAKE) -C nnos a_nask.obj
	$(MAKE) -C winhelo
	$(MAKE) -C winhelo2
	$(MAKE) nnos.img

run_full :
	$(MAKE) full
	$(COPY) nnos.img $(TOOLPATH)qemu/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

run_os :
	$(MAKE) -C nnos
	$(MAKE) run

clean :
# 何もしない

src_only :
	$(MAKE) clean
	-$(DEL) nnos.img

clean_full :
	$(MAKE) -C nnos		clean
	$(MAKE) -C winhelo	clean
	$(MAKE) -C winhelo2	clean

src_only_full :
	$(MAKE) -C nnos		src_only
	$(MAKE) -C winhelo	src_only
	$(MAKE) -C winhelo2	src_only
	-$(DEL) nnos.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) nnos.img
