#include <stdio.h>
#include <string.h>
#include "bootpack.h"

struct BUFFER timer_buf, file_buf;

extern struct localtime lt;

void console_task(struct SHEET *sht, unsigned int memtotal)
{
	struct TASK *task = task_now();
	struct BUFDATA dat;
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color invisible_cursor = {0x00, 0x00, 0x00, 0x00};
	char cmdline[30];
	int i, *fat = task->fat;
	struct CONSOLE cons;
	struct FILEHANDLE fhandle[8];
	unsigned char *nihongo = (char *)*((int *)0x0fe8);
	cons.sht = sht;
	cons.cur_x =  8;
	cons.cur_y = 28;
	cons.start_y = cons.cur_y;
	cons.cur_c = invisible_cursor;
	cons.ccolor = white;
	cons.bcolor = black;
	task->cons = &cons;

	if(cons.sht != 0){
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->buf, 1);
		timer_settime(cons.timer, 50);
	}

	buffer_init(&timer_buf, 32, task);
	buffer_init(&file_buf, 4096, 0);

	for(i = 0; i < 8; i++) fhandle[i].buf = 0;
	task->fhandle = fhandle;

	if(nihongo[4096] != 0xff) task->langmode = 1;
	else task->langmode = 0;
	task->langbyte1 = 0;

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
				if(dat.data == 2) cons.cur_c = cons.ccolor;
				if(dat.data == 3){
					if(cons.sht != 0){
						boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), black, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
						cons.cur_c = invisible_cursor;
					}
				}
				if(dat.data == 4) cmd_exit(&cons);
				if(dat.data == 5){
					/* 前のカーソルが見えちゃうので消す */
					if(cons.cur_x < cons.sht->bxsize && cons.cur_y < cons.sht->bysize)
					boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), black, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);

					make_textbox(sht, 8, 28, sht->bxsize - 16, sht->bysize - 35, cons.bcolor);
					putfontstr_sht(sht, 8, 28, cons.ccolor, cons.bcolor, ">");
					sheet_refresh(cons.sht, 0, 0, cons.sht->bxsize, cons.sht->bysize);

					cons.cur_x = 16;
					cons.cur_y = 28;
				}
			}
			if(dat.tag == TAG_KEYBOARD){
				if(dat.data == 8){
					if(cons.cur_y == cons.start_y){
						if(cons.cur_x > 16){
							cons_putchar(&cons, ' ', 0);
							cons.cur_x -= 8;
						}
					}else{
						if(cons.cur_x > 8){
							cons_putchar(&cons, ' ', 0);
							cons.cur_x -= 8;
						}else{
							/* カーソルが残ることがあるので対策 */
							boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), cons.bcolor, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
							sheet_refresh(sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
							cons.cur_y -= 16;
							cons.cur_x = cons.sht->bxsize - 16;
							cons_putchar(&cons, ' ', 0);
						}
					}
				}else if(dat.data == 10){
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					task->cmdline = cmdline;
					cons_newline(&cons);

					cons_runcmd(cmdline, &cons, memtotal, fat);
					if(cons.sht->height == -1) cmd_exit(&cons);

					cons_putchar(&cons, '>', 1);
					cons.start_y = cons.cur_y;
				}else{
					cmdline[cons.cur_x / 8 - 2] = dat.data;
					cons_putchar(&cons, dat.data, 1);
				}
			}
			if(dat.tag == TAG_TIMER){
				if(dat.data == 0 || dat.data == 1){
					timer_init(cons.timer, &task->buf, (dat.data != 0 ? 0 : 1));
					if(cons.cur_c.alpha != 0x00){
						cons.cur_c = (dat.data != 0 ? cons.ccolor : cons.bcolor);
					}
					timer_settime(cons.timer, 50);
				}
			}
			if(cons.sht != 0){
				if(cons.cur_c.alpha != 0x00) boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				sheet_refresh(sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
			}
		}
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move)
{
	char s[2];

	s[0] = chr;
	s[1] = 0;
	if(s[0] == 0x09){
		for(;;){
			if(cons->sht != 0){
				putfontstr_sht_ref(cons->sht, cons->cur_x, cons->cur_y, cons->ccolor, cons->bcolor, " ");
			}
			cons->cur_x += 8;
			if(cons->cur_x == 8 + (cons->sht->bxsize - 16)) cons_newline(cons);
			if(((cons->cur_x - 8) & 0x1f) == 0) break;
		}
	}else if(s[0] == 0x0a){
		cons_newline(cons);
		cons->start_y = cons->cur_y;
	}else if(s[0] == 0x0d);
	else{
		if(cons->sht != 0) putfontstr_sht_ref(cons->sht, cons->cur_x, cons->cur_y, cons->ccolor, cons->bcolor, s);
		if(move){
			cons->cur_x += 8;
			if(cons->cur_x == 8 + (cons->sht->bxsize - 16)) cons_newline(cons);
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{
	int x, y;
	struct TASK *task = task_now();

	if(cons->cur_y < 28 + (cons->sht->bysize - 37 - 32)) cons->cur_y += 16;
	else{
		if(cons->sht != 0){
			for(y = 28; y < 28 + (cons->sht->bysize - 37 - 16); y++){
				for(x = 8; x < 8 + (cons->sht->bxsize - 16); x++){
					cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 0] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 0];
					cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 1] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 1];
					cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 2] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 2];
					cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 3] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 3];
				}
			}

			for(y = 28 + (cons->sht->bysize - 37 - 16); y < 28 + (cons->sht->bysize - 37); y++){
				for(x = 8; x < 8 + (cons->sht->bxsize - 16); x++){
					putPixel(VMODE_WINDOW, cons->sht->buf, WINDOW_SCLINE(cons->sht), x, y, cons->bcolor);
				}
			}

			if(cons->cur_y + 16 > cons->sht->bysize - 2){
				for(y = 28; y < 28 + (cons->sht->bysize - 37 - 32); y++){
					for(x = 8; x < 8 + (cons->sht->bxsize - 16); x++){
						cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 0] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 0];
						cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 1] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 1];
						cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 2] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 2];
						cons->sht->buf[y * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 3] = cons->sht->buf[(y + 16) * WINDOW_SCLINE(cons->sht) + x * VMODE_WINDOW / 8 + 3];
					}
				}

				for(y = 28 + (cons->sht->bysize - 37 - 32); y < 28 + (cons->sht->bysize - 16); y++){
					for(x = 8; x < 8 + (cons->sht->bxsize - 16); x++){
						putPixel(VMODE_WINDOW, cons->sht->buf, WINDOW_SCLINE(cons->sht), x, y, cons->bcolor);
					}
				}

				for(y = 28 + (cons->sht->bysize - 37 - 16); y < 28 + (cons->sht->bysize - 37); y++){
					for(x = 8; x < 8 + (cons->sht->bxsize - 16); x++){
						putPixel(VMODE_WINDOW, cons->sht->buf, WINDOW_SCLINE(cons->sht), x, y, cons->bcolor);
					}
				}

				cons->cur_y -= 32;
			}

			sheet_refresh(cons->sht, 8, 28, 8 + (cons->sht->bxsize - 16), 28 + (cons->sht->bysize - 37));
		}
	}
	cons->cur_x = 8;
	if(task->langmode == 1 && task->langbyte1 != 0) cons->cur_x = 16;
	return;
}

struct color colortbl[8] = {
	{0x00, 0x00, 0x00, 0xff},
	{0xff, 0x00, 0x00, 0xff},
	{0x00, 0xff, 0x00, 0xff},
	{0xff, 0xff, 0x00, 0xff},
	{0x00, 0x00, 0xff, 0xff},
	{0xff, 0x00, 0xff, 0xff},
	{0x00, 0xff, 0xff, 0xff},
	{0xff, 0xff, 0xff, 0xff}
};

void cons_putstr0(struct CONSOLE *cons, char *s)
{
	for(; *s != 0; s++){
		/* ansi escape */
		if(*s == 0x1b && s[1] == '['){
			/* change back/fore ground color */
			if(s[4] == 'm'){
				if('0' <= s[3] && s[3] <= '7'){
					if(s[2] == '3'){
						cons->ccolor = colortbl[s[3] - '0'];

						s += 5;
					}else if(s[2] == '4'){
						cons->bcolor = colortbl[s[3] - '0'];

						s += 5;
					}
				}
			}
		}
		cons_putchar(cons, *s, 1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l)
{
	int i;

	for(i = 0; i < l; i++){
		/* ansi escape */
			if(*s == 0x1b && s[1] == '['){
			/* change back/fore ground color */
			if(s[4] == 'm'){
				if('0' <= s[3] && s[3] <= '7'){
					if(s[2] == '3'){
						cons->ccolor = colortbl[s[3] - '0'];

						i += 5;
					}else if(s[2] == '4'){
						cons->bcolor = colortbl[s[3] - '0'];

						i += 5;
					}
				}
			}
		}
		cons_putchar(cons, s[i], 1);
	}
	return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, unsigned memtotal, int *fat)
{
	if(!strcmp(cmdline, "mem") && cons->sht != 0) cmd_mem(cons, memtotal);
	else if((!strcmp(cmdline, "cls")) && cons->sht != 0) cmd_cls(cons);
	else if(!strcmp(cmdline, "neofetch") && cons->sht != 0) cmd_neofetch(cons, memtotal);
	else if((!strcmp(cmdline, "dir")) && cons->sht != 0) cmd_dir(cons);
	else if(!strcmp(cmdline, "exit")) cmd_exit(cons);
	else if(!strncmp(cmdline, "start ", 6)) cmd_start(cons, cmdline, memtotal);
	else if(!strncmp(cmdline, "ncst ", 5)) cmd_ncst(cons, cmdline, memtotal);
	else if(!strncmp(cmdline, "langmode ", 9) && cons->sht != 0) cmd_langmode(cons, cmdline);
	else if(!strcmp(cmdline, "shutdown")) cmd_shutdown(cons, fat);
	else if(!strncmp(cmdline, "rename ", 7)) cmd_rename(cons, cmdline);
	else if(!strncmp(cmdline, "del ", 4)) cmd_del(cons, cmdline);
	else if(!strncmp(cmdline, "chgbg ", 6) && cons->sht != 0) cmd_chgbg(cons, cmdline);
	else if(!strncmp(cmdline, "chgfg ", 6) && cons->sht != 0) cmd_chgfg(cons, cmdline);
	else if(cmdline[0] != 0){
		if(cmd_app(cons, cmdline) == 0){
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
	if(memman_total(memman) / 1024 >= 1024) sprintf(s, "free    %dMB\n", memman_total(memman) / (1024 * 1024));
	else sprintf(s, "free    %dKB\n", memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	sprintf(s, "losts %d lostsize %d\n\n", memman->losts, memman->lostsize);
	cons_putstr0(cons, s);
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x, y;

	for(y = 28; y < 28 + 128; y++){
		for(x = 8; x < 8 + 240; x++){
			putPixel(VMODE_WINDOW, cons->sht->buf, WINDOW_SCLINE(cons->sht), x, y, cons->bcolor);
		}
	}
	sheet_refresh(cons->sht, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_neofetch(struct CONSOLE *cons, unsigned int memtotal)
{
	char s[50];
	struct color old_bg = cons->bcolor, old_fg = cons->ccolor;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;

	cons_putstr0(cons, " N    N   \e[36mOS\e[37m:NNOS");
	cons_newline(cons);
	cons_putstr0(cons, " N n  N   \e[36mModel\e[37m:x86(after i486)");
	cons_newline(cons);
	cons_putstr0(cons, " N n  N   \e[36mShell\e[37m:Haribote Shell + NNOS commands");
	cons_newline(cons);
	sprintf(s,         " N  n N   \e[36mResolution\e[37m:%dx%d", binfo->scrnx, binfo->scrny);
	cons_putstr0(cons, s);
	cons_newline(cons);
	sprintf(s,         " N  n N   \e[36mMemory\e[37m:%dMB / %dMB", (memtotal / (1024 * 1024)) - (memman_total(memman) / (1024 * 1024)), memtotal / (1024 * 1024));
	cons_putstr0(cons, s);
	cons_newline(cons);
	sprintf(s,         " N    N   \e[40m  \e[41m  \e[42m  \e[43m  \e[44m  \e[45m  \e[46m  \e[47m  \e[40m");
	cons_putstr0(cons, s);
	cons_newline(cons);

	cons_newline(cons);

	cons->ccolor = old_fg;
	cons->bcolor = old_bg;

	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x2600);
	int i, j;
	char s[40];
	struct localtime time;
	int osize = 0;

	if((lt.y0 * 100 + lt.y1) >= 2100){
		char *fatal = "\e[31mFATAL:If you have files that were updated 2108 later, you risk corrupting your file system.";
		cons_putstr0(cons, fatal);
		cons_newline(cons);
	}

	cons_putstr0(cons, "FILENAME EXT  YEAR DATE   SIZE(byte)");
	cons_newline(cons);
	for(i = 0; i < 224; i++){
		if(finfo[i].name[0] == 0x00) break;

		if(finfo[i].name[0] != 0xe5){
			if((finfo[i].type & 0x18) == 0){
				file_gettime(finfo[i], &time);
				sprintf(s, "filename.ext  %02d%02d/%02d/%02d  %7d\n", time.y0, time.y1, time.month, time.day, finfo[i].size);
				for(j = 0; j < 8; j++) s[j] = finfo[i].name[j];
				s[ 9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				cons_putstr0(cons, s);
				osize += finfo[i].size;
			}
		}
	}
	cons_newline(cons);
	sprintf(s, "NNOS using disk:%7dbyte.\n", osize);
	cons_putstr0(cons, s);
	cons_newline(cons);
	return;
}

void cmd_exit(struct CONSOLE *cons)
{
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *)*((int *)0x0fe4);
	struct BUFFER *buf = (struct BUFFER *)*((int *)0x0fec);

	if(cons->sht != 0) timer_cancel(cons->timer);
	io_cli();
	if(cons->sht != 0){
		buffer_put(buf, TAG_CONSOLE, cons->sht - shtctl->sheets0);
	}else{
		buffer_put(buf, TAG_CONSOLE, task - taskctl->tasks0 + 1024);
	}
	io_sti();
	for(;;) task_sleep(task);
}

void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *)*((int *)0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal, cons->sht->task->fat);
	struct BUFFER *buf = &sht->task->buf;
	int i;

	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top, 0);

	for(i = 6; cmdline[i] != 0; i++){
		buffer_put(buf, TAG_KEYBOARD, cmdline[i]);
	}
	buffer_put(buf, TAG_KEYBOARD, 10);
	cons_newline(cons);

	return;
}

void cmd_ncst(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *)*((int *)0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal, cons->sht->task->fat);
	struct BUFFER *buf = &sht->task->buf;
	int i;

	printlog("ncst:%s\n", cmdline);
	for(i = 5; cmdline[i] != 0; i++){
		buffer_put(buf, TAG_KEYBOARD, cmdline[i]);
	}
	buffer_put(buf, TAG_KEYBOARD, 10);
	cons_newline(cons);

	return;
}

void cmd_langmode(struct CONSOLE *cons, char *cmdline)
{
	struct TASK *task = task_now();
	unsigned char mode = cmdline[9] - '0';

	if(mode <= 2) task->langmode = mode;
	else cons_putstr0(cons, "mode number error.\n");
	cons_newline(cons);

	return;
}

void cmd_shutdown(struct CONSOLE *cons, int *fat)
{
	acpi_hlt((struct BOOTINFO *)0x0ff0, fat);

	cons_putstr0(cons, "Shutdown error.\n");
	cons_newline(cons);

	return;
}

void cmd_rename(struct CONSOLE *cons, char *cmdline)
{
	char *n = cmdline + 7, *n2, *n3;
	char fname2[8], fext2[3];
	int l, len2 = 0;
	struct FILEINFO *finfo1, *finfo2;

	/* 次のファイル名の位置を指す */
	n2 = n;
	while(*n2 != '.') n2++;
	while(*n2 != ' ') n2++;
	n3 = n2 + 1;
	*n2 = 0;

	/* 現在のファイル名 */
	char *f1 = n;
	char *f2 = n3;

	for(l = 0; l < strlen(f1); l++){
		if('a' <= f1[l] && f1[l] <= 'z') f1[l] -= 0x20;
	}
	for(l = 0; l < strlen(f2); l++){
		if('a' <= f2[l] && f2[l] <= 'z') f2[l] -= 0x20;
	}

	for(l = 0; f2[l] != '.'; l++) len2++;

	memset(fname2, ' ', 8);

	/* ファイル名の部分をコピー */
	memcpy(fname2, f2, len2);

	/* 拡張子部分コピー */
	memcpy(fext2, f2 + len2 + 1, 3);


	finfo1 = file_search(f1, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
	finfo2 = file_search(f2, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);

	/* 現在のファイル名はあるね */
	/* 2つ目のファイル名はあっちゃだめだよね */
	if(finfo1 != 0 && finfo2 == 0){
		/* コピーするよ */
		memcpy(finfo1->name, fname2, 8);
		memcpy(finfo1->ext, fext2, 3);
	}else cons_putstr0(cons, "command arg error.");

	return;
}

void cmd_del(struct CONSOLE *cons, char *cmdline)
{
	char *f;
	struct FILEINFO *finfo;

	f = cmdline + 4;
	finfo = file_search(f, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);

	if(finfo != 0) finfo->name[0] = 0xe5;
	else cons_putstr0(cons, "command arg error.");

	return;
}

void cmd_chgbg(struct CONSOLE *cons, char *cmdline)
{
	char *c;
	int l;

	c = cmdline + 6;

	/* ざっくり大文字に変換 */
	for(l = 0; l < strlen(c); l++) if(c[l] >= 0x60) c[l] -= 20;

	if(!strcmp(c, "BLACK")) cons->bcolor = colortbl[0];
	if(!strcmp(c, "RED")) cons->bcolor = colortbl[1];
	if(!strcmp(c, "GREEN")) cons->bcolor = colortbl[2];
	if(!strcmp(c, "YELLOW")) cons->bcolor = colortbl[3];
	if(!strcmp(c, "BLUE")) cons->bcolor = colortbl[4];
	if(!strcmp(c, "MAGENTA")) cons->bcolor = colortbl[5];
	if(!strcmp(c, "CYAN")) cons->bcolor = colortbl[6];
	if(!strcmp(c, "WHITE")) cons->bcolor = colortbl[7];
}

void cmd_chgfg(struct CONSOLE *cons, char *cmdline)
{
	char *c;
	int l;

	c = cmdline + 6;

	/* ざっくり大文字に変換 */
	for(l = 0; l < strlen(c); l++) if(c[l] >= 0x60) c[l] -= 20;

	if(!strcmp(c, "BLACK")) cons->ccolor = colortbl[0];
	if(!strcmp(c, "RED")) cons->ccolor = colortbl[1];
	if(!strcmp(c, "GREEN")) cons->ccolor = colortbl[2];
	if(!strcmp(c, "YELLOW")) cons->ccolor = colortbl[3];
	if(!strcmp(c, "BLUE")) cons->ccolor = colortbl[4];
	if(!strcmp(c, "MAGENTA")) cons->ccolor = colortbl[5];
	if(!strcmp(c, "CYAN")) cons->ccolor = colortbl[6];
	if(!strcmp(c, "WHITE")) cons->ccolor = colortbl[7];
}

int cmd_app(struct CONSOLE *cons, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl;
	struct SHEET *sht;
	char name[18], *p, *q;
	int i, segsiz, datsiz, esp, dathrb, appsiz;

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
		appsiz = finfo->size;
		p = file_loadfile2(finfo->clustno, &appsiz, cons->sht->task->fat);

		if(p == 0) goto memory_error;

		if(appsiz >= 36 && !strncmp(p + 4, "Hari", 4) && *p == 0x00){
			segsiz = *((int *)(p + 0x0000));
			esp    = *((int *)(p + 0x000c));
			datsiz = *((int *)(p + 0x0010));
			dathrb = *((int *)(p + 0x0014));

			q = (char *)memman_alloc_4k(memman, segsiz);
			if(q == 0){
				memman_free_4k(memman, (int)p, appsiz);
				goto memory_error;
			}

			task->ds_base = (int)q;
			set_segmdesc(task->ldt + 0, appsiz - 1, (int)p, AR_CODE32_ER + 0x60);
			set_segmdesc(task->ldt + 1, segsiz - 1, (int)q, AR_DATA32_RW + 0x60);

			for(i = 0; i < datsiz; i++) q[esp + i] = p[dathrb + i];

			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));

			shtctl = (struct SHTCTL *)*((int *)0x0fe4);
			for(i = 0; i < MAX_SHEETS; i++){
				sht = &(shtctl->sheets0[i]);
				if((sht->flags & 0x11) == 0x11 && sht->task == task) sheet_free(sht);
			}

			for(i = 0; i < 8; i++){
				if(task->fhandle[i].buf != 0){
					memman_free_4k(memman, (int)task->fhandle[i].buf, task->fhandle[i].size);
					task->fhandle[i].buf = 0;
				}
			}

			timer_cancelall(&task->timer_buf);
			memman_free_4k(memman, (int)q, segsiz);
			task->langbyte1 = 0;
		}else cons_putstr0(cons, ".hrb file format error.\n");
		memman_free_4k(memman, (int)p, appsiz);
		cons_newline(cons);
		return 1;
	}
	return 0;

memory_error:
	cons_putstr0(cons, "memory alloc error.");
	return 0;
}

void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, struct color c);

struct ret
{
	short v0, v1;
};

/* strcatの実装ではバグがおきてしまうので作り直し */
char *stradd(char *s1, char *s2)
{
	char *s = (char *)memman_alloc_4k((struct MEMMAN *)MEMMAN_ADDR, strlen(s1) + strlen(s2) + 1);

	memcpy(s, s1, strlen(s1));
	memcpy(s + strlen(s1), s2, strlen(s2) + 1);

	return s;
}

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *)*((int *)0x0fe4);
	struct SHEET *sht = 0;
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color invisible_cursor = {0x00, 0x00, 0x00, 0x00};
	struct color c;
	struct BUFDATA dat;
	struct BUFFER *sys_buf = (struct BUFFER *)*((int *)0x0fec);
	struct FILEINFO *finfo, *finfo2;
	struct FILEINFO *files;
	struct FILEHANDLE *fh;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	char fname[9], fext[4];
	int ds_base = task->ds_base, i = 0;
	int *reg = &eax + 1;
		/* reg[0] : EDI reg[1] : ESI reg[2] : EBP reg[3] : ESP */
		/* reg[4] ; EBX reg[5] : EDX reg[6] : ECX reg[7] : EAX */

	if(edx == 1) cons_putchar(cons, eax & 0xff, 1);
	if(edx == 2) cons_putstr0(cons, (char *)ebx + ds_base);
	if(edx == 3) cons_putstr1(cons, (char *)ebx + ds_base, ecx);
	if(edx == 4) return &(task->tss.esp0);
	if(edx == 5){
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= 0x10;
		sheet_setbuf(sht, (char *)(ebx + ds_base), esi, edi);
		make_window(sht, (char *)(ecx + ds_base), 0, 0, (char)eax);
		sheet_slide(sht, (shtctl->xsize - esi) / 2, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top, 1);

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
		sheet_refreshmap(sht->ctl, sht->vx0 + eax, sht->vy0 + ecx, sht->vx0 + esi, sht->vy0 + edi, 0);
		sheet_refreshsub(sht->ctl, sht->vx0 + eax, sht->vy0 + ecx, sht->vx0 + esi, sht->vy0 + edi, 0, shtctl->top);
	}
	if(edx == 13){
		sht = (struct SHEET *)(ebx & 0xfffffffe);
		c = *((struct color *)(ebp + ds_base));
		hrb_api_linewin(sht, eax, ecx, esi, edi, c);
		if((ebx & 1) == 0){
			if(eax > esi){
				i = eax;
				eax = esi;
				esi = i;
			}
			if(ecx > edi){
				i = ecx;
				ecx = edi;
				edi = i;
			}
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
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
					if(dat.data == 4){
						timer_cancel(cons->timer);
						io_cli();
						buffer_put(sys_buf, 0, cons->sht - shtctl->sheets0 + 2024);
						cons->sht = 0;
						io_sti();
					}
					if(dat.data == 5){
						/* 前のカーソルが見えちゃうので消す */
						if(cons->cur_x < cons->sht->bxsize && cons->cur_y < cons->sht->bysize)
						boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), black, cons->cur_x, cons->cur_y, cons->cur_x + 7, cons->cur_y + 15);
						sheet_refresh(cons->sht, cons->cur_x, cons->cur_y, cons->cur_x + 8, cons->cur_y + 16);
						cons->cur_x = 16;
						cons->cur_y = 28;
					}
					if(dat.data == 6){
						reg[7] = 1;
						return 0;
					}
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
		buffer_init(&task->timer_buf, 32, task);

		reg[7] = (int)timer_alloc();
		((struct TIMER *)reg[7])->flags2 = 1;
	}
	if(edx == 17) timer_init((struct TIMER *)ebx, &task->timer_buf, eax);
	if(edx == 18) timer_settime((struct TIMER *)ebx, eax);
	if(eax == 19) timer_free((struct TIMER *)ebx);
	if(edx == 20){
		for(;;){
			io_cli();
			if(buffer_status(&task->timer_buf) == 0){
				if(eax != 0) task_sleep(task);
				else{
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}else{
				dat = buffer_get(&task->timer_buf);

				reg[7] = dat.data;
				io_sti();
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
	if(edx == 22){
		for(i = 0; i < 8; i++){
			if(task->fhandle[i].buf == 0) break;
		}

		fh = &task->fhandle[i];
		reg[7] = 0;
		if(i < 8){
			finfo = file_search((char *)ebx + ds_base, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
			if(finfo != 0){
				/* ファイル読み込み処理 */
				reg[7] = (int)fh;
				fh->size = finfo->size;
				fh->pos = 0;
				fh->buf = file_loadfile2(finfo->clustno, &fh->size, cons->sht->task->fat);
<<<<<<< HEAD
			}else{
				/* ファイル作成処理 */
				files = (struct FILEINFO *)(ADR_DISKIMG + 0x2600);
=======
				fh->finfo = finfo;
			}else if(esi == 1){
				/* ファイル作成処理 */
				struct FILEINFO *files = (struct FILEINFO *)(ADR_DISKIMG + 0x2600);
>>>>>>> developer

				/* ファイル情報を書く場所を探す */
				for(i = 0; i < 224; i++){
					if(files[i].name[0] == 0x00) break;
				}

				if(i != 224){
<<<<<<< HEAD
					finfo = &files[i];
					finfo2 = &files[i - 1];
=======
					struct FILEINFO *finfo = &files[i];

					char fname[9], fext[4];
>>>>>>> developer

					/* ファイル名をいったんすべて空白に */
					memset(fname, ' ', 8);
					memset(fext, ' ', 3);
					fname[8] = 0;
					fext[3] = 0;

					char *filename = (char *)(ebx + ds_base);
					int fnlen = 0, fextlen = 0;

<<<<<<< HEAD
					printlog("file:%s\n", filename);

					/* .が来るまで(拡張子にたどり着くまで)待つ */
					for(; filename[fnlen] != '.'; fnlen++);
						for(; filename[fnlen + 1 + fextlen] != 0; fextlen++);

					printlog("name:%d len ext:%d len\n", fnlen, fextlen);
=======
					/* .が来るまで(拡張子にたどり着くまで)待つ */
					for(; filename[fnlen] != '.'; fnlen++);
					/* 拡張子の長さをはかる */
					for(; filename[fnlen + 1 + fextlen] != 0; fextlen++);
>>>>>>> developer

					for(i = 0; i < strlen(filename); i++){
						/* ファイル名を大文字に変換 */
						if('a' <= filename[i] && filename[i] <= 'z') filename[i] -= 0x20;
					}

					memcpy(fname, filename, fnlen);
					memcpy(fext, filename + fnlen + 1, fextlen);

					memcpy(finfo->name, fname, 8);
					memcpy(finfo->ext, fext, 3);
					/* 空きFATを検索する */
					finfo->clustno = fat_findfree(cons->sht->task->fat);
					cons->sht->task->fat[finfo->clustno] = FAT_USING;
					finfo->date = ((((lt.y0 * 100 + lt.y1) - 1980) & 0xef) << 9) | ((lt.month & 0x0f) << 5) | (lt.day & 0x1f);
					finfo->time = ((lt.hour & 0x0f) << 11) | ((lt.min & 0x1f) << 5) | (lt.sec / 2);
					finfo->size = 0;

					reg[7] = (int)fh;
					fh->size = finfo->size;
					fh->pos = 0;
<<<<<<< HEAD
					/* ファイル書き込み */
					fh->buf = (char *)(ADR_DISKIMG + 0x3e00 + (finfo->clustno * 512));
				/* エラー */
				}else reg[7] = 0;
=======
					fh->buf = (char *)(ADR_DISKIMG + 0x3e00 + (finfo->clustno * 512));
					fh->finfo = &finfo[0];
				}
>>>>>>> developer
			}
		}
	}
	if(edx == 23){
		fh = (struct FILEHANDLE *)eax;
		memman_free_4k(memman, (int)fh->buf, fh->size);
		fh->buf = 0;
	}
	if(edx == 24){
		fh = (struct FILEHANDLE *)eax;
		if(ecx == 0) fh->pos = ebx;
		if(ecx == 1) fh->pos += ebx;
		if(ecx == 2) fh->pos = fh->size + ebx;
		if(fh->pos < 0) fh->pos = 0;
		if(fh->pos > fh->size) fh->pos = fh->size;
	}
	if(edx == 25){
		fh = (struct FILEHANDLE *)eax;
		if(ecx == 0) reg[7] = fh->size;
		if(ecx == 1) reg[7] = fh->pos;
		if(ecx == 2) reg[7] = fh->pos - fh->size;
	}
	if(edx == 26){
		fh = (struct FILEHANDLE *)eax;
		for(i = 0; i < ecx; i++){
			if(fh->pos == fh->size) break;
			printlog("%02X\n", (char)fh->buf[fh->pos]);
			*((char *)ebx + ds_base + i) = fh->buf[fh->pos];
			fh->pos++;
		}
		reg[7] = i;
	}
	if(edx == 27){
		char *cmdline = task->cmdline;
		/* ncst を飛ばす */
		if(cons->sht == 0) cmdline += 5;
		for(;;){
			*((char *)ebx + ds_base + i) = cmdline[i];
			if(task->cmdline[i] == 0) break;
			if(i >= ecx) break;
			i++;
		}
		reg[7] = i;
	}
	if(edx == 28) reg[7] = task->langmode;
	/* fwrite */
<<<<<<< HEAD
	if(edx == 29){
		fh = (struct FILEHANDLE *)eax;
		char *buf = (char *)(ebx + ds_base);
		reg[7] = 0;

		if(ecx > 512) ecx = 512;
		if(ecx <= 0) reg[7] = -1;

		if(reg[7] == 0){
			if(fh->pos == 0 && fh->size == 0){
				/* コピー */
				/* 初めからなので上書き */
				memcpy(fh->buf, buf, ecx);
				fh->size = ecx;
				fh->pos = ecx;
				reg[7] = ecx;
			/* 512バイトまでに入りきる */
			}else if(fh->pos + ecx <= 512){
				memcpy(fh->buf + fh->pos, buf, ecx);
				// サイズは書き換えなので変わらない
				fh->pos += ecx;
				reg[7] = ecx;
			}
=======
	/* memo:メモリに書き込むだけならディスクバッファに書く */
	/* ディスクに書くときはFATをもとに戻すのを忘れずに */
	if(edx == 29){
		struct FILEHANDLE *fh = (struct FILEHANDLE *)eax;

//		int write, old = fh->finfo->clustno, fat0;
		char *buf0 = (char *)(ebx + ds_base);

		/* ファイルの外を指している */
		/* バグの原因になるので終わりにする */
		if(fh->pos < 0 && fh->pos > fh->size) return 0;

		reg[7] = 0;

		/* 新しくファイルに書き込む */
		if(fh->pos == 0 && fh->size == 0){
			reg[7] = file_write0(fh, buf0, ecx, cons->sht->task->fat);
		}else if(fh->pos == 0 && fh->size != 0){
			int old = fh->finfo->clustno, fat0;
			int fat = cons->sht->task->fat[old];

			while(1){
				/* 1クラスタ0で埋めてリセット */
				memset((char *)(ADR_DISKIMG + 0x3e00 + old * 512), 0, 512);

				fat0 = fat;

				if(fat0 == FAT_USING) break;

				/* 次のFATを探す */
				fat = cons->sht->task->fat[fat0];

				/* FAT解放(最後の部分のFATのみ残る) */
				cons->sht->task->fat[fat0] = FAT_UNUSED;
				old = fat;
			}

			fh->finfo->clustno = fat0;
			fh->size = 0;
			fh->finfo->size = fh->size;
			fh->buf = (char *)(ADR_DISKIMG + 0x3e00 + fh->finfo->clustno * 512);

			/* ファイルを書き込む */
			reg[7] = file_write0(fh, buf0, ecx, cons->sht->task->fat);
		/* 追記部分を実装しようとしたらバグでうまく読み込めないので */
		/* コメント化してそのまま置いておく */
		}else if(fh->pos != 0 && fh->size != 0){
			fh->size = fh->pos;
			fh->finfo->size = fh->size;
			reg[7] = file_write00(fh, buf0, ecx, cons->sht->task->fat);
			/**** ここのサイズ計算も間違いになるので直す */
			fh->size = fh->pos;
			fh->finfo->size = fh->size;
//			printlog("size:%d\n", fh->size);
//			printlog("fsize:%d\n", fh->finfo->size);
>>>>>>> developer
		}
	}
	if(edx == 30){
		struct localtime *time = (struct localtime *)(ebx + ds_base);
		time->y0 = lt.y0;
		time->y1 = lt.y1;
		time->month = lt.month;
		time->day = lt.day;
		time->hour = lt.hour;
		time->min = lt.min;
		time->sec = lt.sec;
	}
	if(edx == 31){
		sht = (struct SHEET *)ebx;
		if(eax == 0){
			struct ret *r = (struct ret *)(esi + ds_base);
			r->v0 = (short)sht->bxsize;
			r->v1 = (short)sht->bysize;
		}
		if(eax == 1){
			struct ret *r = (struct ret *)(esi + ds_base);
			r->v0 = (short)sht->vx0;
			r->v1 = (short)sht->vy0;
		}
	}
	/* exec application */
	if(edx == 32){
		char *arg = (char *)(char *)(ebx + ds_base);

<<<<<<< HEAD
		printlog("%s\n\n", arg);

=======
>>>>>>> developer
		unsigned int memtotal = memtest(ADR_BOTPAK, 0xffffffff);
		cmd_ncst(cons, stradd("ncst ", arg), memtotal);
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

void close_constask(struct TASK *task)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int)task->buf.buf, 128 * 4);
	task->flags = 0;
	return;
}

void close_console(struct SHEET *sht)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int)sht->buf, 256 * 165 * 4);
	sheet_free(sht);
	close_constask(task);
	return;
}
