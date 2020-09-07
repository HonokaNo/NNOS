/* naskfunc.nas */
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
void load_tr(int tr);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);
void clts(void);
void fnsave(int *addr);
void frstor(int *addr);

/* asminterrupt.nas */
void asm_inthandler07(void);
void asm_inthandler0c(void);
void asm_inthandler0d(void);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler28(void);
void asm_inthandler2c(void);

void asm_hrb_api(void);

#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG		0x00100000

struct BOOTINFO
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
	short scline;
};

void printlog(char *format, ...);
/* JPEG */
struct DLL_STRPICENV
{
	int work[64 * 1024 / 4];
};

int info_JPEG(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_JPEG(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);

/* windowに描画関数を実行する場合のvmode */
#define VMODE_WINDOW	32
#define WINDOW_SCLINE(sht) (sht->bxsize * VMODE_WINDOW / 8)

struct color
{
	unsigned char r, g, b;
	/* 0:透明 それ以外:不透明 */
	char alpha;
};

void init_screen(char vmode, char *vram, int scline, int scrnx, int scrny, struct color back);

void putPixel(char vmode, char *vram, int scline, int x, int y, struct color c);
void boxfill(char vmode, char *vram, int scline, struct color c, int x0, int y0, int x1, int y1);
void putfont(char vmode, char *vram, int scline, int x, int y, struct color c, char *font);
void putfontstr(char vmode, char *vram, int scline, int x, int y, struct color c, unsigned char *s);
int eqColor(struct color c1, struct color c2);
unsigned char rgb2pal(int r, int g, int b, int x, int y);

void setPalette(void);

struct localtime
{
	/* XXYY XX:y0 YY:y1 */
	unsigned char y0, y1;
	unsigned char month, day;
	unsigned char hour, min, sec;
	unsigned char dayofweek;
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
void init_pic(void);

#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_LDT			0x0082
#define AR_TSS32		0x0089
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
#define TAG_TIMER		4		/* タイマ割り込み */
#define TAG_CONSOLE		5		/* コンソール終了データ */

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
	struct TASK *task;
};

void buffer_init(struct BUFFER *buf, int size, struct TASK *task);
int buffer_put(struct BUFFER *buf, int tag, int data);
struct BUFDATA buffer_get(struct BUFFER *buf);
int buffer_status(struct BUFFER *buf);

struct MOUSE_DEC
{
	unsigned char buf[3], phase;
	int x, y, btn;
};

#define MEMMAN_FREES		1500
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

#define MAX_SHEETS	100

struct SHEET
{
	unsigned char *buf, *title;
	int bxsize, bysize, vx0, vy0, height, flags;
	/* 作成時のサイズ */
	int mix, miy, wx, wy;
	struct SHTCTL *ctl;
	struct TASK *task;
	/* bs:最大化しているか resize:最大化できるようにするか */
	char bs, resize;
};

struct SHTCTL
{
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
/* ウィンドウサイズ変更時使用 最大化していないときのサイズを保存しない */
void sheet_resetbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize);
/* 最初にウィンドウを作成するとき使用 最大化していないときのサイズを保存 */
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

/* graphic.c */
void init_mouse_cursor(struct SHEET *sht);

void send_data(unsigned char data);
void send_string(char *string);

#define MAX_TIMER	300
#define TIMER_FLAGS_ALLOC	1
#define TIMER_FLAGS_USING	2

struct TIMER
{
	struct TIMER *next;
	unsigned int timeout;
	char flags, flags2;
	struct BUFFER *buf;
	int data;
};

struct TIMERCTL
{
	unsigned int count, next;
	struct TIMER *t0;
	struct TIMER *timers0;
};

extern struct TIMERCTL timerctl;

void init_pit(void);
void init_timerctl(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct BUFFER *buf, unsigned char data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
int timer_cancel(struct TIMER *timer);
void timer_cancelall(struct BUFFER *buf);

#define MAX_TASKS		500
#define TASK_GDT0		  3
#define MAX_TASKS_LV	100
#define MAX_TASKLEVELS	 10

struct TSS32
{
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct TASK
{
	int sel, flags;
	int level, priority;
	struct TSS32 tss;
	struct BUFFER buf;
	struct SEGMENT_DESCRIPTOR ldt[2];
	struct CONSOLE *cons;
	int ds_base, cons_stack;
	struct FILEHANDLE *fhandle;
	char *cmdline;
	int langmode, langbyte1;
	int *fat;
	int fpu_register[108 / 4];
};

struct TASKLEVEL
{
	int running, now;
	struct TASK *tasks[MAX_TASKS_LV];
};

struct TASKCTL
{
	int now_lv;
	char lv_change;
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
	struct TASK *fpu_task;
};

extern struct TASKCTL *taskctl;
extern struct TIMER *task_timer;

struct TASK *task_now(void);
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int level, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);

struct CONSOLE
{
	struct SHEET *sht;
	int cur_x, cur_y;
	struct color cur_c;
	struct TIMER *timer;
};

struct FILEHANDLE
{
	char *buf;
	int size;
	int pos;
};

void console_task(struct SHEET *sht, unsigned int memtotal);
void cons_putchar(struct CONSOLE *cons, int chr, char move);
void cons_newline(struct CONSOLE *cons);
void cons_putstr0(struct CONSOLE *cons, char *s);
void cons_putstr1(struct CONSOLE *cons, char *s, int l);
void cons_runcmd(char *cmdline, struct CONSOLE *cons, unsigned memtotal);
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);
void cmd_cls(struct CONSOLE *cons);
void cmd_neofetch(struct CONSOLE *cons);
void cmd_dir(struct CONSOLE *cons);
void cmd_exit(struct CONSOLE *cons);
void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal);
void cmd_ncst(struct CONSOLE *cons, char *cmdline, int memtotal);
void cmd_langmode(struct CONSOLE *cons, char *cmdline);
int cmd_app(struct CONSOLE *cons, char *cmdline);
int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

struct FILEINFO
{
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};

void file_readfat(int *fat, unsigned char *img);
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
char *file_loadfile2(int clustno, int *psize, int *fat);

void make_wtitle(struct SHEET *sht, char *title, char act, char bs, char resize);
void make_window(struct SHEET *sht, char *title, char act, char bs, char resize);
void putfontstr_sht(struct SHEET *sht, int x, int y, struct color c, struct color bc, char *s);
void putfontstr_sht_ref(struct SHEET *sht, int x, int y, struct color c, struct color bc, char *s);
void make_textbox(struct SHEET *sht, int x0, int y0, int sx, int sy, struct color c);
struct color getColorWin(struct SHEET *sht, int bx, int by);
void change_wtitle(struct SHEET *sht, char act);
void window_resize(struct SHEET *sht, int nx, int ny);

void wait_KBC_sendready(void);
void init_keyboard(void);
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal, int *fat);
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal, int *fat);

int tek_getsize(unsigned char *p);
int tek_decomp(unsigned char *p, char *q, int size);
