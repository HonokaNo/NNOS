void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
unsigned int memtest_sub(unsigned int start, unsigned int end);

void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler28(void);
void asm_inthandler2c(void);

struct BOOTINFO
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
	short scline;
};

/* windowに描画関数を実行する場合のvmode */
#define VMODE_WINDOW	32
#define WINDOW_SCLINE(sht) (sht->bxsize * 4)

struct color
{
	unsigned char r, g, b;
	/* 0:透明 それ以外:不透明 */
	char alpha;
};

void init_screen(char vmode, char *vram, int scline, int scrnx, int scrny);

void putPixel(char vmode, char *vram, int scline, int x, int y, struct color c);
void boxfill(char vmode, char *vram, int scline, struct color c, int x0, int y0, int x1, int y1);
void putfont(char vmode, char *vram, int scline, int x, int y, struct color c, char *font);
void putfontstr(char vmode, char *vram, int scline, int x, int y, struct color c, unsigned char *s);

void setPalette(void);

struct localtime
{
	unsigned short year;
	unsigned char month, day;
	unsigned char hour, min, sec;
};

int readcmos(unsigned char addr);
struct localtime readrtc();

struct SEGMENT_DESCRIPTOR
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR
{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define TAG_RESERVE		0		/* 空き領域/エラー data:0xff=データなし */
#define TAG_KEYBOARD	1		/* キーボードデータ */
#define TAG_RTC			2		/* RTC時刻更新 */
#define TAG_MOUSE		3		/* マウスデータ */

struct BUFDATA
{
	/* データ種別 */
	int tag;
	int data;
};

struct BUFFER
{
	struct BUFDATA *buf;
	char flg;
	int size, free, r, w;
};

void buffer_init(struct BUFFER *buf, int size);
int buffer_put(struct BUFFER *buf, int tag, int data);
struct BUFDATA buffer_get(struct BUFFER *buf);
int buffer_status(struct BUFFER *buf);

struct MOUSE_DEC
{
	unsigned char buf[3], phase;
	int x, y, btn;
};

#define MEMMAN_FREES		3000
#define MEMMAN_ADDR			0x003c0000

struct FREEINFO
{
	unsigned int addr, size;
};

struct MEMMAN
{
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);

#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064

#define MAX_SHEETS	256

struct SHEET
{
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, height, flags;
	struct SHTCTL *ctl;
};

struct SHTCTL
{
	unsigned char *vram;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize);
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

/* graphic.c */
void init_mouse_cursor(struct SHEET *sht);
