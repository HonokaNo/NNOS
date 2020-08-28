#include "bootpack.h"
#include <stdio.h>
#include <string.h>

void console_task(struct SHEET *sht, unsigned int memtotal)
{
	struct TASK *task = task_now();
	struct BUFDATA dat;
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color invisible_cursor = {0x00, 0x00, 0x00, 0x00};
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	char cmdline[30];
	int *fat = (int *)memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	cons.sht = sht;
	cons.cur_x =  8;
	cons.cur_y = 28;
	cons.cur_c = invisible_cursor;
	task->cons = &cons;

	cons.timer = timer_alloc();
	timer_init(cons.timer, &task->buf, 1);
	timer_settime(cons.timer, 50);

	file_readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x200));

	cons_putchar(&cons, '>', 1);

	for(;;){
		io_cli();
		if(buffer_status(&task->buf) == 0){
			task_sleep(task);
			io_sti();
		}else{
			dat = buffer_get(&task->buf);
			io_sti();
			if(dat.tag == 0){
				if(dat.data == 2) cons.cur_c = white;
				if(dat.data == 3){
					boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), black, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
					cons.cur_c = invisible_cursor;
				}
			}
			if(dat.tag == TAG_KEYBOARD){
				if(dat.data == 8){
					if(cons.cur_x > 16){
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				}else if(dat.data == 10){
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);

					cons_runcmd(cmdline, &cons, fat, memtotal);

					cons_putchar(&cons, '>', 1);
				}else{
					if(cons.cur_x < 240){
						cmdline[cons.cur_x / 8 - 2] = dat.data;
						cons_putchar(&cons, dat.data, 1);
					}
				}
			}
			if(dat.tag == TAG_TIMER){
				if(dat.data == 0 || dat.data == 1){
					timer_init(cons.timer, &task->buf, (dat.data != 0 ? 0 : 1));
					if(cons.cur_c.alpha != 0x00){
						cons.cur_c = (dat.data != 0 ? white : black);
					}
					timer_settime(cons.timer, 50);
				}
			}
			if(cons.cur_c.alpha != 0x00) boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			sheet_refresh(sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
		}
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move)
{
	char s[2];
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color white = {0xff, 0xff, 0xff, 0xff};

	s[0] = chr;
	s[1] = 0;
	if(s[0] == 0x09){
		for(;;){
			putfontstr_sht_ref(cons->sht, cons->cur_x, cons->cur_y, white, black, " ");
			cons->cur_x += 8;
			if(cons->cur_x == 8 + 240) cons_newline(cons);
			if(((cons->cur_x - 8) & 0x1f) == 0) break;
		}
	}else if(s[0] == 0x0a) cons_newline(cons);
	else if(s[0] == 0x0d);
	else{
		putfontstr_sht_ref(cons->sht, cons->cur_x, cons->cur_y, white, black, s);
		if(move){
			cons->cur_x += 8;
			if(cons->cur_x == 8 + 240) cons_newline(cons);
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{
	int x, y;
	struct color black = {0x00, 0x00, 0x00, 0xff};

	if(cons->cur_y < 28 + 112) cons->cur_y += 16;
	else{
		for(y = 28; y < 28 + 112; y++){
			for(x = 8; x < 8 + 240; x++){
				cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 0] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 0];
				cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 1] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 1];
				cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 2] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 2];
				cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 3] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 3];
			}
		}

		for(y = 28 + 112; y < 28 + 128; y++){
			for(x = 8; x < 8 + 240; x++){
				putPixel(VMODE_WINDOW, cons->sht->buf, WINDOW_SCLINE(cons->sht), x, y, black);
			}
		}

		sheet_refresh(cons->sht, 8, 28, 8 + 240, 28 + 128);
	}
	cons->cur_x = 8;
	return;
}

void cons_putstr0(struct CONSOLE *cons, char *s)
{
	for(; *s != 0; s++) cons_putchar(cons, *s, 1);
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l)
{
	int i;

	for(i = 0; i < l; i++) cons_putchar(cons, s[i], 1);
	return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned memtotal)
{
	if(!strcmp(cmdline, "mem")) cmd_mem(cons, memtotal);
	else if(!strcmp(cmdline, "cls") || !strcmp(cmdline, "clear")) cmd_cls(cons);
	else if(!strcmp(cmdline, "neofetch")) cmd_neofetch(cons);
	else if(!strcmp(cmdline, "dir") || !strcmp(cmdline, "ls")) cmd_dir(cons);
	else if(!strncmp(cmdline, "type ", 5) || !strcmp(cmdline, "cat")) cmd_type(cons, fat, cmdline);
	else if(!strcmp(cmdline, "exit")) cmd_exit(cons, fat);
	else if(cmdline[0] != 0){
		if(cmd_app(cons, fat, cmdline) == 0){
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
	return;
}

void cmd_mem(struct CONSOLE *cons, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	char s[30];

	if(memtotal / (1024 * 1024) >= 1024) sprintf(s, "total   %dGB\n", memtotal / (1024 * 1024 * 1024));
	else sprintf(s, "total   %dMB\n", memtotal / (1024 * 1024));
	cons_putstr0(cons, s);
	if(memman_total(memman) / 1024 >= 1024) sprintf(s, "free    %dMB\n\n", memman_total(memman) / (1024 * 1024));
	else sprintf(s, "free    %dKB\n\n", memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x, y;
	struct color black = {0x00, 0x00, 0x00, 0xff};

	for(y = 28; y < 28 + 128; y++){
		for(x = 8; x < 8 + 240; x++){
			putPixel(VMODE_WINDOW, cons->sht->buf, WINDOW_SCLINE(cons->sht), x, y, black);
		}
	}
	sheet_refresh(cons->sht, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_neofetch(struct CONSOLE *cons)
{
	char s[30];
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color light_blue = {0x00, 0xff, 0xff, 0xff};

	sprintf(s, "OS:NNOS 1.0");
	putfontstr_sht_ref(cons->sht, 8 + 8 * 8, cons->cur_y, light_blue, black, s);
	cons_newline(cons);
	sprintf(s, "Model:x86(after i486)");
	putfontstr_sht_ref(cons->sht, 8 + 8 * 8, cons->cur_y, light_blue, black, s);
	cons_newline(cons);

	cons_newline(cons);
	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x2600);
	int i, j;
	char s[30];

	for(i = 0; i < 224; i++){
		if(finfo[i].name[0] == 0x00) break;

		if(finfo[i].name[0] != 0xe5){
			if((finfo[i].type & 0x18) == 0){
				sprintf(s, "filename.ext   %7d\n", finfo[i].size);
				for(j = 0; j < 8; j++) s[j] = finfo[i].name[j];
				s[ 9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(cmdline + 5, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
	char *p;

	if(finfo != 0){
		p = (char *)memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKIMG + 0x3e00));
		cons_putstr1(cons, p, finfo->size);
		memman_free_4k(memman, (int)p, finfo->size);
	}else cons_putstr0(cons, "File not found.\n");
	cons_newline(cons);
	return;
}

void cmd_exit(struct CONSOLE *cons, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *)*((int *)0x0fe4);
	struct BUFFER *buf = (struct BUFFER *)*((int *)0x0fec);

	timer_cancel(cons->timer);
	memman_free_4k(memman, (int)fat, 4 * 2880);
	io_cli();
	buffer_put(buf, TAG_CONSOLE, cons->sht - shtctl->sheets0);
	io_sti();
	for(;;) task_sleep(task);
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl;
	struct SHEET *sht;
	char name[18], *p, *q;
	int i, segsiz, datsiz, esp, dathrb;

	for(i = 0; i < 13; i++){
		if(cmdline[i] <= ' ') break;
		name[i] = cmdline[i];
	}
	name[i] = 0;

	finfo = file_search(name, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
	if(finfo == 0 && name[i - 1] != '.'){
		name[i + 0] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
	}

	if(finfo != 0){
		p = (char *)memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKIMG + 0x3e00));

		if(finfo->size >= 36 && !strncmp(p + 4, "Hari", 4) && *p == 0x00){
			segsiz = *((int *)(p + 0x0000));
			esp    = *((int *)(p + 0x000c));
			datsiz = *((int *)(p + 0x0010));
			dathrb = *((int *)(p + 0x0014));

			q = (char *)memman_alloc_4k(memman, segsiz);
			task->ds_base = (int)q;
			set_segmdesc(gdt + task->sel / 8 + 1000, finfo->size - 1, (int)p, AR_CODE32_ER + 0x60);
			set_segmdesc(gdt + task->sel / 8 + 2000, segsiz - 1,      (int)q, AR_DATA32_RW + 0x60);

			for(i = 0; i < datsiz; i++) q[esp + i] = p[dathrb + i];

			start_app(0x1b, task->sel + 1000 * 8, esp, task->sel + 2000 * 8, &(task->tss.esp0));

			shtctl = (struct SHTCTL *)*((int *)0x0fe4);
			for(i = 0; i < MAX_SHEETS; i++){
				sht = &(shtctl->sheets0[i]);
				if((sht->flags & 0x11) == 0x11 && sht->task == task) sheet_free(sht);
			}
			timer_cancelall(&task->buf);
			memman_free_4k(memman, (int)q, segsiz);
		}else cons_putstr0(cons, ".hrb file format error.\n");
		memman_free_4k(memman, (int)p, finfo->size);
		cons_newline(cons);
		return 1;
	}
	return 0;
}

void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, struct color c);

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *)*((int *)0x0fe4);
	struct SHEET *sht;
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color invisible_cursor = {0x00, 0x00, 0x00, 0x00};
	struct color c;
	struct BUFDATA dat;
	struct BUFFER timer_buf;
	int ds_base = task->ds_base, i;
	int *reg = &eax + 1;
		/* reg[0] : EDI reg[1] : ESI reg[2] : EBP reg[3] : ESP */
		/* reg[4] ; EBX reg[5] : EDX reg[6] : ECX reg[7] : EAX */

	buffer_init(&timer_buf, 32, task);

	if(edx == 1) cons_putchar(cons, eax & 0xff, 1);
	if(edx == 2) cons_putstr0(cons, (char *)ebx + ds_base);
	if(edx == 3) cons_putstr1(cons, (char *)ebx + ds_base, ecx);
	if(edx == 4) return &(task->tss.esp0);
	if(edx == 5){
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= 0x10;
		sheet_setbuf(sht, (char *)ebx + ds_base, esi, edi);
		make_window(sht, (char *)(ecx + ds_base), 0);
		sheet_slide(sht, (shtctl->xsize - esi) / 2, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top);
		reg[7] = (int)sht;
	}
	if(edx == 6){
		sht = (struct SHEET *)(ebx & 0xfffffffe);
		c = *((struct color *)(eax + ds_base));
		putfontstr(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), esi, edi, c, (char *)(ebp + ds_base));
		sheet_refresh(sht, esi, edi, esi + strlen((char *)(ebp + ds_base)) * 8, edi + 16);
	}
	if(edx == 7){
		sht = (struct SHEET *)(ebx & 0xfffffffe);
		c = *((struct color *)(ebp + ds_base));
		boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), c, eax, ecx, esi, edi);
		sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
	}
	if(edx == 8){
		memman_init((struct MEMMAN *)(ebx + ds_base));
		ecx &= 0xfffffff0;
		memman_free((struct MEMMAN *)(ebx + ds_base), eax, ecx);
	}
	if(edx == 9){
		ecx = (ecx + 0x0f) & 0xfffffff0;
		reg[7] = memman_alloc((struct MEMMAN *)(ebx + ds_base), ecx);
	}
	if(edx == 10){
		ecx = (ecx + 0x0f) & 0xfffffff0;
		memman_free((struct MEMMAN *)(ebx + ds_base), eax, ecx);
	}
	if(edx == 11){
		sht = (struct SHEET *)(ebx & 0xfffffffe);
		c = *((struct color *)(eax + ds_base));
		putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), esi, edi, c);
		if((ebx & 1) == 0) sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
	}
	if(edx == 12){
		sht = (struct SHEET *)ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	}
	if(edx == 13){
		sht = (struct SHEET *)(ebx & 0xfffffffe);
		c = *((struct color *)(ebp + ds_base));
		hrb_api_linewin(sht, eax, ecx, esi, edi, c);
		if((ebx & 1) == 0) sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
	}
	if(edx == 14) sheet_free((struct SHEET *)ebx);
	if(edx == 15){
		for(;;){
			io_cli();
			if(buffer_status(&task->buf) == 0){
				if(eax != 0) task_sleep(task);
				else{
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}else{
				dat = buffer_get(&task->buf);

				io_sti();
				if(dat.tag == 0){
					if(dat.data == 2) cons->cur_c = white;
					if(dat.data == 3) cons->cur_c = invisible_cursor;
				}else if(dat.tag == TAG_KEYBOARD){
					reg[7] = dat.data;
					return 0;
				}else if(dat.tag == TAG_TIMER){
					if(dat.data == 0 || dat.data == 1){
						timer_init(cons->timer, &task->buf, 1);
						timer_settime(cons->timer, 50);
					}
				}
			}
		}
	}
	if(edx == 16){
		reg[7] = (int)timer_alloc();
		((struct TIMER *)reg[7])->flags2 = 1;
	}
	if(edx == 17) timer_init((struct TIMER *)ebx, &timer_buf, eax);
	if(edx == 18) timer_settime((struct TIMER *)ebx, eax);
	if(eax == 19) timer_free((struct TIMER *)ebx);
	if(edx == 20){
		for(;;){
			io_cli();
			if(buffer_status(&timer_buf) == 0){
				if(eax != 0) task_sleep(task);
				else{
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}else{
				dat = buffer_get(&timer_buf);

				io_sti();
				reg[7] = dat.data;
				return 0;
			}
		}
	}
	if(edx == 21){
		if(eax == 0){
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		}else{
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	}

	return 0;
}

void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, struct color c)
{
	int i, x, y, dx, dy, len;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if(dx < 0) dx = -dx;
	if(dy < 0) dy = -dy;

	if(dx >= dy){
		len = dx + 1;

		if(x0 > x1) dx = -1024;
		else dx = 1024;

		if(y0 <= y1) dy = ((y1 - y0 + 1) << 10) / len;
		else dy = ((y1 - y0 - 1) << 10) / len;
	}else{
		len = dy + 1;

		if(y0 > y1) dy = -1024;
		else dy = 1024;

		if(x0 <= x1) dx = ((x1 - x0 + 1) << 10) / len;
		else dx = ((x1 - x0 - 1) << 10) / len;
	}

	for(i = 0; i < len; i++){
		putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), (x >> 10), (y >> 10), c);
		x += dx;
		y += dy;
	}
}
