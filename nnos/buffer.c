#include "bootpack.h"

void buffer_init(struct BUFFER *buf, int size)
{
	struct MEMMAN *man = (struct MEMMAN *)MEMMAN_ADDR;

	buf->buf = (struct BUFDATA *)memman_alloc_4k(man, sizeof(struct BUFDATA) * size);
	buf->size = size;
	buf->free = size;
	buf->r = buf->w = 0;
	buf->flg = 0;
}

int buffer_put(struct BUFFER *buf, int tag, int data)
{
	if(buf->free == 0){
		buf->flg = 1;
		return -1;
	}
	buf->buf[buf->w].tag = tag;
	buf->buf[buf->w].data = data;
	buf->w++;
	if(buf->w == buf->size) buf->w = 0;
	buf->free--;

	return 0;
}

struct BUFDATA buffer_get(struct BUFFER *buf)
{
	struct BUFDATA data;

	if(buf->free == buf->size){
		data.tag = 0;
		data.data = 0xff;
		return data;
	}
	data = buf->buf[buf->r];
	buf->r++;
	if(buf->r == buf->size) buf->r = 0;
	buf->free++;

	return data;
}

int buffer_status(struct BUFFER *buf)
{
	return buf->size - buf->free;
}
