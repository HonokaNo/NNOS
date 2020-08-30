include configure.txt

# デフォルト動作

default :
	$(MAKE) nnos.img

# ファイル生成規則

nnos.img : nnos/ipl.bin nnos/nnos.sys Makefile \
		winhelo/winhelo.hrb winhelo2/winhelo2.hrb winhelo3/winhelo3.hrb \
		star1/star1.hrb stars/stars.hrb stars2/stars2.hrb \
		lines/lines.hrb walk/walk.hrb hello5/hello5.hrb \
		noodle/noodle.hrb beepdown/beepdown.hrb beepup/beepup.hrb \
		sosu/sosu.hrb type/type.hrb iroha/iroha.hrb chklang/chklang.hrb
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
		copy from:walk/walk.hrb to:@: \
		copy from:hello5/hello5.hrb to:@: \
		copy from:noodle/noodle.hrb to:@: \
		copy from:beepdown/beepdown.hrb to:@: \
		copy from:beepup/beepup.hrb to:@: \
		copy from:sosu/sosu.hrb to:@: \
		copy from:type/type.hrb to:@: \
		copy from:iroha/iroha.hrb to:@: \
		copy from:chklang/chklang.hrb to:@: \
		copy from:nihongo/nihongo.fnt to:@: \
		copy from:euc.txt to:@: \
		imgout:nnos.img

# コマンド

run :
	$(MAKE) nnos.img
	$(COPY) nnos.img $(TOOLPATH)qemu/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

full :
	$(MAKE) -C nnos
	$(MAKE) -C library
	$(MAKE) -C winhelo
	$(MAKE) -C winhelo2
	$(MAKE) -C winhelo3
	$(MAKE) -C star1
	$(MAKE) -C stars
	$(MAKE) -C stars2
	$(MAKE) -C lines
	$(MAKE) -C walk
	$(MAKE) -C hello5
	$(MAKE) -C noodle
	$(MAKE) -C beepdown
	$(MAKE) -C beepup
	$(MAKE) -C sosu
	$(MAKE) -C type
	$(MAKE) -C iroha
	$(MAKE) -C chklang
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
	$(MAKE) -C library	clean
	$(MAKE) -C winhelo	clean
	$(MAKE) -C winhelo2	clean
	$(MAKE) -C winhelo3	clean
	$(MAKE) -C star1	clean
	$(MAKE) -C stars	clean
	$(MAKE) -C stars2	clean
	$(MAKE) -C lines	clean
	$(MAKE) -C walk		clean
	$(MAKE) -C hello5	clean
	$(MAKE) -C noodle	clean
	$(MAKE) -C beepdown	clean
	$(MAKE) -C beepup	clean
	$(MAKE) -C sosu		clean
	$(MAKE) -C type		clean
	$(MAKE) -C iroha	clean
	$(MAKE) -C chklang	clean

src_only_full :
	$(MAKE) -C nnos		src_only
	$(MAKE) -C library	src_only
	$(MAKE) -C winhelo	src_only
	$(MAKE) -C winhelo2	src_only
	$(MAKE) -C winhelo3	src_only
	$(MAKE) -C star1	src_only
	$(MAKE) -C stars	src_only
	$(MAKE) -C stars2	src_only
	$(MAKE) -C lines	src_only
	$(MAKE) -C walk		src_only
	$(MAKE) -C hello5	src_only
	$(MAKE) -C noodle	src_only
	$(MAKE) -C beepdown	src_only
	$(MAKE) -C beepup	src_only
	$(MAKE) -C sosu		src_only
	$(MAKE) -C type		src_only
	$(MAKE) -C iroha	src_only
	$(MAKE) -C chklang	src_only
	-$(DEL) nnos.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) nnos.img
