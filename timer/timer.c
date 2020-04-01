//timer registers u32 offset from base address
#define TLOAD 0x0
#define TVALUE 0x1
#define TCNTL 0x2
#define TINTCLR 0x3
#define TRIS 0x4
#define TMIS 0x5
#define TBGLOAD 0x6

int strcp(char *dest, char *src) {
	while(*src) {
		*dest++ = *src++;
	}
	*dest = 0;
}

typedef volatile struct timer{
	unsigned int *base;
	int tick, hh, mm, ss;
	char clock[16];
}TIMER;

typedef struct tqe {
	struct tqe *next;
	int seconds;
	int event;
	PROC *p;
}TQE;



extern PROC *running;
TQE timer_list[8];
volatile TIMER timer0;

void add_tqe(int seconds, int event) {
	int i;
	for(i = 0; i < 8; i++) {
		if(timer_list[i].event == 0){
			timer_list[i].event = event; 
			timer_list[i].seconds = seconds;
			timer_list[i].p = running;
			break;
		}
	}
}

void timer_init(){
	int i; TIMER *tp = &timer0;
	printf("Timer init\n");
	
	tp->base = 0x101E2000;
	*(tp->base + TLOAD) = 0x0;
	*(tp->base + TVALUE) = 0xFFFFFFFF;
	*(tp->base + TRIS) = 0x0;
	*(tp->base + TMIS) = 0x0;
	*(tp->base + TLOAD) = 0x100;
	*(tp->base + TCNTL) = 0x66;
	*(tp->base + TBGLOAD) = 0x1C00;

	tp->tick = tp->hh = tp->mm = tp->ss = 0;
	tp->clock[2] = tp->clock[5] = ':';
}

void timer_handler() {
	int i; TIMER *t = &timer0;
	t->tick++;

	if(t->tick == 10) {

		for(i = 0; i < 8; i++) {
			if(timer_list[i].event != 0) {
				timer_list[i].seconds--;
				if(timer_list[i].seconds == 0) {
					printf("timer calling wakeup with event %d\n", timer_list[i].event);
					kwakeup(timer_list[i].event);
					timer_list[i].event = 0;
				}
			}
		}
		t->tick = 0;
		t->ss++;
		if(t->ss == 60) {
			t->ss = 0;
			t->mm++;
			if(t->mm == 60){
				t->mm = 0;
				t->hh++;
			}
		}
		t->clock[7] = '0' + (t->ss % 10);
		t->clock[6] = '0' + (t->ss / 10);
		t->clock[4] = '0' + (t->mm % 10);
		t->clock[3] = '0' + (t->mm / 10);
		t->clock[1] = '0' + (t->hh % 10);
		t->clock[0] = '0' + (t->hh / 10);
   

  
		for(i = 0; i < 8; i++){
			if(timer_list[i].event != 0)
			    printf("[%d %d ]->", timer_list[i].p->pid,timer_list[i].seconds);
				
		}
	}

	for(i = 0; i < 8; i++){
		kpchar(t->clock[i], 0, 70 + i);
	}
	timer_clearInterrupt();
}

void timer_start() {
	TIMER *tp = &timer0;
	kprintf("timer_start base = %x\n", tp->base);
	*(tp->base + TCNTL) |= 0x80;
}

int timer_clearInterrupt() {
	TIMER *tp = &timer0;
	*(tp->base + TINTCLR) = 0xFFFFFFFF;
}

void timer_stop() {
	TIMER *tp = &timer0;
	*(tp->base + TCNTL) &= 0x7F;
}




