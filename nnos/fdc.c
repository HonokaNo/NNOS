#include "bootpack.h"

#define MOTOR_STOP_SEC 3

char motor;

extern char intflg;

void fdc_init(void)
{
	io_out8(0xd6, 0xc0);	/* �}�X�^��ch0���J�X�P�[�h���[�h�� */
	io_out8(0xc0, 0x00);	/* �X���[�u��DMA������ */
	io_out8(0x0a, 0x06);	/* �}�X�^��ch2��DMA���}�X�N */

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
	/* ���荞�݂��N����܂ő҂� */
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

/* FDC�ɑ���R�}���h */
struct BUFFER FDCbuffer;

/* �ǂݍ���/�������݂��I��������Ƃ������t���O */
int rwflg = 0;

char rw_end = 0;

void fdc_write(unsigned int cylinder, unsigned int head, unsigned int sector, unsigned int addr)
{
	char[] cmd = {0xc5, head << 2, cylinder, head, sector, 0x02, 0x7f, 0x12, 0xff};
	char result[7];

	/* �������ݗp�ݒ� */
	io_out8(0x0b, 0x0a);
	io_out8(0x05, 0xff);
	io_out8(0x05, 0x01);		/* 1�Z�N�^�������� */
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

	/* ���[�^�[��~ */
	io_out8(0x3f2, 0x0c);
	motor = 0;

	for(;;){
		io_cli();

		if(buffer_status(&FDCbuffer) == 0){
			/* 3s�҂� */
			timer_settime(timer, MOTOR_STOP_SEC * 100);
			task_sleep(task);
			io_sti();

			/* �������݂��Ȃ� */
			rw_end = 1;
		}else{
			dat = buffer_get(&FDCbuffer);
			io_sti();

			if(dat.tag == TAG_TIMER){
				if(dat.data == 128){
					if(rw == 0){
						/* 3s�������̂Ń��[�^�[��~ */
						stop_motor();
						task_sleep(task);
					}else if(rw == 2){
						fdc_write(c, h, s, addr);

						rw = 0;
					}
				}
			}else{
				timer_cancel(timer);

				/* �K�v�Ȉ��������ׂĂ��邱�Ƃ��m�F���� */
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

							/* ���[�^�[���� */
							start_motor();
							timer_settime(timer, 300);
						}
					}
				}
			}
		}
	}
}
