include configure.txt

# デフォルト動作

default :
	$(MAKE) nnos.img

# ファイル生成規則

nnos.img : nnos/ipl.bin nnos/nnos.sys Makefile \
		winhelo/winhelo.hrb winhelo2/winhelo2.hrb winhelo3/winhelo3.hrb \
		star1/star1.hrb stars/stars.hrb stars2/stars2.hrb \
		lines/lines.hrb
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:nnos/ipl.bin len:512 from:0 to:0 \
		copy from:nnos/nnos.sys to:@: \
		copy from:nnos/ipl.nas to:@: \
		copy from:nnos/make.bat to:@: \
		copy from:winhelo/winhelo.hrb to:@: \
		copy from:winhelo2/winhelo2.hrb to:@: \
		copy from:winhelo3/winhelo3.hrb to:@: \
		copy from:star1/star1.hrb to:@: \
		copy from:stars/stars.hrb to:@: \
		copy from:stars2/stars2.hrb to:@: \
		copy from:lines/lines.hrb to:@: \
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
	$(MAKE) -C winhelo3
	$(MAKE) -C star1
	$(MAKE) -C stars
	$(MAKE) -C stars2
	$(MAKE) -C lines
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
	$(MAKE) -C winhelo3	clean
	$(MAKE) -C star1	clean
	$(MAKE) -C stars	clean
	$(MAKE) -C stars2	clean
	$(MAKE) -C lines	clean

src_only_full :
	$(MAKE) -C nnos		src_only
	$(MAKE) -C winhelo	src_only
	$(MAKE) -C winhelo2	src_only
	$(MAKE) -C winhelo3	src_only
	$(MAKE) -C star1	src_only
	$(MAKE) -C stars	src_only
	$(MAKE) -C stars2	src_only
	$(MAKE) -C lines	src_only
	-$(DEL) nnos.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) nnos.img
