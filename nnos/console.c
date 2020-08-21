#include "bootpack.h"
#include <stdio.h>
#include <string.h>

void console_task(struct SHEET *sht, unsigned int memtotal)
{
	struct TASK *task = task_now();
	struct TIMER *timer;
	struct BUFDATA dat;
	struct color black = {0x00, 0x00, 0x00, 0xff};
	struct color white = {0xff, 0xff, 0xff, 0xff};
	struct color invisible_cursor = {0x00, 0x00, 0x00, 0x00};
	struct color light_green = {0x00, 0xff, 0xff, 0xff};
	struct color cursor_c = invisible_cursor;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x2600);
	char s[30], cmdline[30], *p;
	int cursor_x = 16, cursor_y = 28, x, y;
	int *fat = (int *)memman_alloc_4k(memman, 4 * 2880);

	buffer_init(&task->buf, 128, task);

	timer = timer_alloc();
	timer_init(timer, &task->buf, 1);
	timer_settime(timer, 50);

	file_readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x200));

	putfontstr_sht_ref(sht, 8, 28, white, black, ">");

	for(;;){
		io_cli();
		if(buffer_status(&task->buf) == 0){
			task_sleep(task);
			io_sti();
		}else{
			dat = buffer_get(&task->buf);
			io_sti();
			if(dat.tag == 0){
				if(dat.data == 2) cursor_c = white;
				if(dat.data == 3){
					boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), black, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
					cursor_c = invisible_cursor;
				}
			}
			if(dat.tag == TAG_KEYBOARD){
				if(dat.data == 8){
					if(cursor_x > 16){
						putfontstr_sht_ref(sht, cursor_x, cursor_y, white, black, " ");
						cursor_x -= 8;
					}
				}else if(dat.data == 10){
					putfontstr_sht_ref(sht, cursor_x, cursor_y, white, black, " ");
					cmdline[cursor_x / 8 - 2] = 0;
					cursor_y = cons_newline(cursor_y, sht);

					if(!strcmp(cmdline, "mem")){
						if(memtotal / (1024 * 1024) >= 1024) sprintf(s, "total   %dGB", memtotal / (1024 * 1024 * 1024));
						else sprintf(s, "total   %dMB", memtotal / (1024 * 1024));
						putfontstr_sht_ref(sht, 8, cursor_y, white, black, s);
						cursor_y = cons_newline(cursor_y, sht);
						if(memman_total(memman) / 1024 >= 1024) sprintf(s, "free    %dMB", memman_total(memman) / (1024 * 1024));
						else sprintf(s, "free %dKB", memman_total(memman) / 1024);
						putfontstr_sht_ref(sht, 8, cursor_y, white, black, s);
						cursor_y = cons_newline(cursor_y, sht);
						cursor_y = cons_newline(cursor_y, sht);
					}else if(!strcmp(cmdline, "cls") || !strcmp(cmdline, "clear")){
						for(y = 28; y < 28 + 128; y++){
							for(x = 8; x < 8 + 240; x++){
								putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), x, y, black);
							}
						}
						sheet_refresh(sht, 8, 28, 8 + 240, 28 + 128);
						cursor_y = 28;
					}else if(!strcmp(cmdline, "neofetch")){
						sprintf(s, "OS:NNOS 1.0");
						putfontstr_sht_ref(sht, 8 + 8 * 8, cursor_y, light_green, black, s);
						cursor_y = cons_newline(cursor_y, sht);
						sprintf(s, "Model:x86(after i486)");
						putfontstr_sht_ref(sht, 8 + 8 * 8, cursor_y, light_green, black, s);
						cursor_y = cons_newline(cursor_y, sht);

						cursor_y = cons_newline(cursor_y, sht);
					}else if(!strcmp(cmdline, "dir")){
						for(x = 0; x < 224; x++){
							if(finfo[x].name[0] == 0x00) break;

							if(finfo[x].name[0] != 0xe5){
								if((finfo[x].type & 0x18) == 0){
									sprintf(s, "filename.ext   %7d", finfo[x].size);
									for(y = 0; y < 8; y++){
										s[y] = finfo[x].name[y];
									}
									s[ 9] = finfo[x].ext[0];
									s[10] = finfo[x].ext[1];
									s[11] = finfo[x].ext[2];
									putfontstr_sht_ref(sht, 8, cursor_y, white, black, s);
									cursor_y = cons_newline(cursor_y, sht);
								}
							}
						}
						cursor_y = cons_newline(cursor_y, sht);
					}else if(!strncmp(cmdline, "type ", 5)){
						for(y = 0; y < 11; y++) s[y] = ' ';
						y = 0;

						for(x = 5; y < 11 && cmdline[x] != 0; x++){
							if(cmdline[x] == '.' && y <= 8) y = 8;
							else{
								s[y] = cmdline[x];
								if('a' <= s[y] && s[y] <= 'z') s[y] -= 0x20;
								y++;
							}
						}

						for(x = 0; x < 224;){
							if(finfo[x].name[0] == 0x00) break;
							if((finfo[x].type & 0x18) == 0){
								for(y = 0; y < 11; y++){
									if(finfo[x].name[y] != s[y]) goto type_next_file;
								}
								break;
							}
type_next_file:
							x++;
						}

						if(x < 224 && finfo[x].name[0] != 0x00){
							p = (char *)memman_alloc_4k(memman, finfo[x].size);
							file_loadfile(finfo[x].clustno, finfo[x].size, p, fat, (char *)(ADR_DISKIMG + 0x3e00));
							cursor_x = 8;
							for(x = 0; x < y; x++){
								s[0] = p[x];
								s[1] = 0;
								if(s[0] == 0x09){
									for(;;){
										putfontstr_sht_ref(sht, cursor_x, cursor_y, white, black, " ");
										cursor_x += 8;
										if(cursor_x == 8 + 240){
											cursor_x = 8;
											cursor_y = cons_newline(cursor_y, sht);
										}
										if(((cursor_x - 8) & 0x1f) == 0) break;
									}
								}else if(s[0] == 0x0a){
									cursor_x = 8;
									cursor_y = cons_newline(cursor_y, sht);
								}else if(s[0] == 0x0d);
								else{
									putfontstr_sht_ref(sht, cursor_x, cursor_y, white, black, s);
									cursor_x += 8;
									if(cursor_x == 8 + 240){
										cursor_x = 8;
										cursor_y = cons_newline(cursor_y, sht);
									}
								}
							}
						}else{
							putfontstr_sht_ref(sht, 8, cursor_y, white, black, "File not found.");
							cursor_y = cons_newline(cursor_y, sht);
						}
						cursor_y = cons_newline(cursor_y, sht);
					}else if(cmdline[0] != 0){
						putfontstr_sht_ref(sht, 8, cursor_y, white, black, "Bad command.");
						cursor_y = cons_newline(cursor_y, sht);
						cursor_y = cons_newline(cursor_y, sht);
					}

					putfontstr_sht_ref(sht, 8, cursor_y, white, black, ">");
					cursor_x = 16;
				}else{
					if(cursor_x < 240){
						s[0] = dat.data;
						s[1] = 0;
						cmdline[cursor_x / 8 - 2] = dat.data;
						putfontstr_sht_ref(sht, cursor_x, cursor_y, white, black, s);
						cursor_x += 8;
					}
				}
			}
			if(dat.tag == TAG_TIMER){
				if(dat.data == 0 || dat.data == 1){
					if(cursor_c.alpha != 0x00){
						timer_init(timer, &task->buf, (dat.data != 0 ? 0 : 1));
						cursor_c = (dat.data != 0 ? white : black);
					}
					timer_settime(timer, 50);
				}
			}
			if(cursor_c.alpha != 0x00) boxfill(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), cursor_c, cursor_x, cursor_y, cursor_x + 7, cursor_y + 15);
			sheet_refresh(sht, cursor_x, cursor_y, cursor_x + 8, cursor_y + 16);
		}
	}
}

int cons_newline(int cursor_y, struct SHEET *sht)
{
	struct color black = {0x00, 0x00, 0x00, 0xff};
	int x, y;

	if(cursor_y < 28 + 112) cursor_y += 16;
	else{
		for(y = 28; y < 28 + 112; y++){
			for(x = 8; x < 8 + 240; x++){
				sht->buf[y * WINDOW_SCLINE(sht) + x * VMODE_WINDOW / 8 + 0] = sht->buf[(y + 16) * WINDOW_SCLINE(sht) + x * VMODE_WINDOW / 8 + 0];
				sht->buf[y * WINDOW_SCLINE(sht) + x * VMODE_WINDOW / 8 + 1] = sht->buf[(y + 16) * WINDOW_SCLINE(sht) + x * VMODE_WINDOW / 8 + 1];
				sht->buf[y * WINDOW_SCLINE(sht) + x * VMODE_WINDOW / 8 + 2] = sht->buf[(y + 16) * WINDOW_SCLINE(sht) + x * VMODE_WINDOW / 8 + 2];
				sht->buf[y * WINDOW_SCLINE(sht) + x * VMODE_WINDOW / 8 + 3] = sht->buf[(y + 16) * WINDOW_SCLINE(sht) + x * VMODE_WINDOW / 8 + 3];
			}
		}

		for(y = 28 + 112; y < 28 + 128; y++){
			for(x = 8; x < 8 + 240; x++){
				putPixel(VMODE_WINDOW, sht->buf, WINDOW_SCLINE(sht), x, y, black);
			}
		}

		sheet_refresh(sht, 8, 28, 8 + 240, 28 + 128);
	}

	return cursor_y;
}
