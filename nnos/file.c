#include <string.h>
#include "bootpack.h"

void file_readfat(int *fat, unsigned char *img)
{
	int i, j = 0;

	for(i = 0; i < 2880; i += 2){
		fat[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;

		j += 3;
	}
	return;
}

void file_writefat(int *fat, unsigned char *img)
{
	int i, j = 0;

	for(i = 0; i < 2880; i += 2){
		img[j + 0] = (fat[i + 0] & 0xff);
		img[j + 1] = (fat[j + 1] & 0x0f) << 4;
		img[j + 1] = (fat[j + 0] & 0xf00) >> 8;
		img[j + 2] = (fat[j + 1] >> 4) & 0xff;

		j += 3;
	}
	return;
}

int fat_findfree(int *fat)
{
	int l = 0;

	for(; l < 2880; l++){
		if(fat[l] == 0x00) return l;
	}

	return -1;
}

void file_loadfile(int clustno, int size, char *buf, char *img, int *fat)
{
	int i;

	for(;;){
		if(size <= 512){
			for(i = 0; i < size; i++) buf[i] = img[clustno * 512 + i];
			break;
		}

		for(i = 0; i < 512; i++) buf[i] = img[clustno * 512 + i];

		size -= 512;
		buf += 512;
		clustno = fat[clustno];
	}
	return;
}

struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max)
{
	int i, j;
	char s[12];

	for(j = 0; j < 11; j++) s[j] = ' ';
	j = 0;

	for(i = 0; name[i] != 0; i++){
		if(j >= 11) return 0;
		if(name[i] == '.' && j <= 8) j = 8;
		else{
			s[j] = name[i];
			if('a' <= s[j] && s[j] <= 'z') s[j] -= 0x20;
			j++;
		}
	}

	for(i = 0; i < max;){
		if(finfo[i].name[0] == 0x00) break;
		if((finfo[i].type & 0x18) == 0){
			for(j = 0; j < 11; j++){
				if(finfo[i].name[j] != s[j]) goto next;
			}
			return finfo + i;
		}
next:
		i++;
	}
	return 0;
}

char *file_loadfile2(int clustno, int *psize, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	int size = *psize, size2;
	char *buf, *buf2;

	buf = (char *)memman_alloc_4k(memman, size);
	if(buf == 0) return 0;

	file_loadfile(clustno, size, buf, (char *)(ADR_DISKIMG + 0x3e00), fat);
	if(size >= 17){
		size2 = tek_getsize(buf);
		if(size2 > 0){
			buf2 = (char *)memman_alloc_4k(memman, size2);
			if(buf2 == 0) return 0;

			tek_decomp(buf, buf2, size2);
			memman_free_4k(memman, (int)buf, size);
			buf = buf2;
			*psize = size2;
		}
	}
	return buf;
}

void file_gettime(struct FILEINFO finfo, struct localtime *lt)
{
	// rshiftは最上位ビットをコピーしてしまう(符号保持のため)
	// なのでコピーされた符号を消してあげる
	short y = ((finfo.date >> 9) & 0x7f) + 1980;
	short m = ((finfo.date << 7 >> 12) & 0x0f);
	short d = (finfo.date & 0x1f);

	lt->y0 = (y / 100);
	lt->y1 = (y % 100);

	lt->month = (char)m;
	lt->day = (char)d;

	lt->hour = (char)(finfo.time >> 10);
	lt->min = (char)(finfo.time << 6 >> 11);
	lt->sec = (char)(finfo.time & 0x0f) * 2;
}

struct CHS
{
	char cylinder;
	char head;
	char sector;
};

void getCHS(unsigned int addr, struct CHS *chs)
{
	/* アドレスからのオフセット */
	unsigned int offset = addr - ADR_DISKIMG;

	char sectors = offset / 512;

	return;
}

int file_write0(struct FILEHANDLE *fh, const char *buf0, int len, int *fat0)
{
	extern struct BUFFER FDCbuffer;
	int write, old = fh->finfo->clustno;

	/* 書き込むデータがある間繰り返す */
	while(len > 0){
		/* 1クラスタには収まらないので1クラスタ分だけ書く */
		if(len > 512) write = 512;
		else write = len;

		/* バッファに書き込む */
		memcpy(fh->buf, buf0, write);

		len -= write;
		fh->size += write;
		fh->finfo->size += write;
		buf0 += write;

		/* 書き込むデータはなくなったので終了 */
		if(len <= 0) break;

		int fat = fat_findfree(fat0);
		if(fat == -1) return -1;
		else{
			fat0[fat] = FAT_USING;
			fat0[old] = fat;

			old = fat;
		}
	}

	return 0;
}

int file_write00(struct FILEHANDLE *fh, const char *buf0, int len, int *fat0)
{
	int write, old = fh->finfo->clustno, start = fh->pos, tmp = fh->pos, len0 = len;

	while(len0 > 512 - tmp){
		len0 -= 512 - tmp;
		/* 次のFATの場所に飛ぶ */
		old = fat0[old];
		/* バッファをコピー */
		buf0 = (char *)(ADR_DISKIMG + 0x3e00 + old * 512);
		tmp = 0;
	}

	/* とんだクラスタ内でのオフセット */
	fh->pos = len0;

	/* 書き込むデータがある間繰り返す */
	while(len > 0){
		/* 1クラスタには収まらないので1クラスタ分だけ書く */
		if(len > 512 - fh->pos) write = 512 - fh->pos;
		else write = len;

		/* バッファに書き込む */
		memcpy(fh->buf + fh->pos, buf0, write);

		len -= write;
		fh->size += write;
		fh->finfo->size += write;
		buf0 += write;
		start += write;

		/* 書き込むデータはなくなったので終了 */
		if(len <= 0) break;

		fh->pos = 0;

		int fat = fat_findfree(fat0);
		if(fat == -1) return -1;
		else{
			fat0[fat] = FAT_USING;
			fat0[old] = fat;

			old = fat;
		}
	}

	fh->pos = start;

	return 0;
}
