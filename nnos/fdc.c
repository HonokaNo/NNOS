#include "bootpack.h"

#define MOTOR_STOP_SEC 3

char motor;

extern char intflg;

void fdc_init(void)
{
	io_out8(0xd6, 0xc0);	/* マスタのch0をカスケードモードに */
	io_out8(0xc0, 0x00);	/* スレーブのDMAを許可 */
	io_out8(0x0a, 0x06);	/* マスタのch2のDMAをマスク */

	return;
}

void fdc_sendcmd(char *cmd, int len)
{
	int l;

	while((io_in8(0x3f4) & 0x11) != 0);

	for(l = 0; l < len; l++){
		while((io_in8(0x3f4) & 0xc0) != 0x80);

		io_out8(0x3f5, cmd[l]);
	}

	return;
}

void fdc_result(char *result, int len)
{
	int l;

	for(l = 0; l < len; l++){
		while((io_in8(0x3f4) & 0xc0) != 0xc0);

		result[l] = io_in8(0x3f5);
	}

	return;
}

void wait_interrupt(void)
{
	/* 割り込みが起こるまで待つ */
	while(!intflg);

	return;
}

void start_motor(void)
{
	if(!motor){
		io_out8(0x3f2, 0x1c);
		motor = 1;
	}

	return;
}

void stop_motor(void)
{
	if(motor){
		io_out8(0x3f2, 0x0c);
		motor = 0;
	}

	return;
}

/* FDCに送るコマンド */
struct BUFFER FDCbuffer;

/* 読み込み/書き込みが終わったことを示すフラグ */
int rwflg = 0;

char rw_end = 0;

void fdc_write(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int addr)
{
	char[] cmd = {0xc5, head << 2, cylinder, head, sector, 0x02, 0x7f, 0x12, 0xff};
	char result[7];

	/* 書き込み用設定 */
	io_out8(0x0b, 0x0a);
	io_out8(0x05, 0xff);
	io_out8(0x05, 0x01);		/* 1セクタ書き込み */
	io_out8(0x04, addr & 0xff);
	io_out8(0x04, (addr >> 8) & 0xff);
	io_out8(0x81, (addr >> 16) & 0xff);
	io_out8(0x0a, 0x02);

	fdc_sendcmd(cmd, 9);

	wait_interrupt();

	fdc_result(result, 7);

	io_out8(0x0a, 0x06);

	rwflg = 1;

	return;
}

static char rw = 0;

void task_fdc(void)
{
	struct TASK *task = task_now();
	struct TIMER *timer;
	struct BUFDATA dat;
	/* cylinder, head, sector */
	unsigned int c, h, s, addr;

	buffer_init(&FDCbuffer, 512, 0);

	timer = timer_alloc();
	timer_init(timer, 128, 0);

	/* モーター停止 */
	io_out8(0x3f2, 0x0c);
	motor = 0;

	for(;;){
		io_cli();

		if(buffer_status(&FDCbuffer) == 0){
			/* 3s待つ */
			timer_settime(timer, MOTOR_STOP_SEC * 100);
			task_sleep(task);
			io_sti();

			/* 書き込みがない */
			rw_end = 1;
		}else{
			dat = buffer_get(&FDCbuffer);
			io_sti();

			if(dat.tag == TAG_TIMER){
				if(dat.data == 128){
					if(rw == 0){
						/* 3s立ったのでモーター停止 */
						stop_motor();
						task_sleep(task);
					}else if(rw == 2){
						fdc_write(c, h, s, addr);

						rw = 0;
					}
				}
			}else{
				timer_cancel(timer);

				/* 必要な引数がすべてあることを確認する */
				if(buffer_status(&FDCbuffer) >= 4){
					if(dat.tag == TAG_FDC){
						/* read mode */
						if(dat.data == 1){
							c = (unsigned int)buffer_get(&FDCbuffer);
							h = (unsigned int)buffer_get(&FDCbuffer);
							s = (unsigned int)buffer_get(&FDCbuffer);
							addr = (unsigned int)buffer_get(&FDCbuffer);
						/* write mode */
						}else if(dat.data == 2){
							c = (unsigned int)buffer_get(&FDCbuffer);
							h = (unsigned int)buffer_get(&FDCbuffer);
							s = (unsigned int)buffer_get(&FDCbuffer);
							addr = (unsigned int)buffer_get(&FDCbuffer);

							rw = 2;

							/* モーターを回す */
							start_motor();
							timer_settime(timer, 300);
						}
					}
				}
			}
		}
	}
}
