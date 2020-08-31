APP    = gview
STACK  = 6920k
MALLOC = 0k

include ../app_make.txt

$(APP).bim : $(APP).obj bmp.obj jpeg.obj $(APILIBPATH)nnlib.lib Makefile ../app_make.txt
	$(OBJ2BIM) @$(RULEFILE) out:$(APP).bim map:$(APP).map stack:$(STACK) \
		$(APP).obj $(APILIBPATH)nnlib.lib bmp.obj jpeg.obj

clean:
	-$(DEL) *.lst
	-$(DEL) gview.obj
	-$(DEL) jpeg.obj
	-$(DEL) *.bim
	-$(DEL) *.map
