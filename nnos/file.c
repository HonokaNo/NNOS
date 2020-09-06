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
