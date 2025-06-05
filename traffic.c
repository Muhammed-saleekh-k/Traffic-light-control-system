#include<pic.h>
#define _XTAL_FREQ 20000000

// macros for PATH 1
#define red1    RB0
#define orange1 RB1
#define green1  RB2
#define button1 RB3

// macros for PATH 2
#define red2    RB4
#define orange2 RB5
#define green2  RB6
#define button2 RB7 

// macros for LCD
#define rs RC0
#define en RC1

void delay0();
void delay2();
void delay();

void cmd(unsigned char a);
void data (unsigned char b);
void str(unsigned char *s);
void lcd(); 
void lcd_delay();

 // buttons initial condition
int button1state = 0;
int button2state = 0;



void main()
{
	TRISB = 0x00;
	PORTB = 0x00;
	TRISD = 0x00;
	TRISC = 0x00;
	TRISA = 0x00;
	

	// prescaler setting for TMR 0
	OPTION_REG = 0x07;//presacler assigned to TMR0 , prescaler = 256
	
	// prescaler and postscaler setting of TMR 2
	T2CON = 0x7b; // postscaler = 16, prescaler = 16 ,TMR2 OFF

	button1 = 0; // initially button not pressed
	button2 = 0; // initially button not pressed
	
	lcd();
	while (1)
{
	if(button1state == 0 && button2state == 0)
{
	red1 = green2 = 1;
	red2 = green1 = 0;
	orange1 = orange2 = 0;
	cmd(0x80);
	str("PATH 1: RED");
	cmd(0xc0);
	str("PATH 2: GREEN");
	delay();

	red1 = green2 = 0;
	red2 = green1 = 0;
	orange1 = orange2 = 1;
	cmd(0x01);
	cmd(0x80);
	str("PATH 1: ORANGE");
	cmd(0xc0);
	str("PATH 2: ORANGE");
	delay2();

	red1 = green2 = 0;
	red2 = green1 = 1;
	orange1 = orange2 = 0;
	cmd(0x01);
	cmd(0x80);
	str("PATH 1: GREEN");
	cmd(0xc0);
	str("PATH 2: RED");
	delay();

	red1 = green2 = 0;
	red2 = green1 = 0;
	orange1 = orange2 = 1;
	cmd(0x01);
	cmd(0x80);
	str("PATH 1: ORANGE");
	cmd(0xc0);
	str("PATH 2: ORANGE");
	delay2();
}	
if (button1state ==1 || button2state == 1)
{
	
	orange1 = orange2 = 0;
	red1 = red2 = 1;
	cmd(0x01);
	cmd(0x80);
	str("PATH 1: RED");
	cmd(0xc0);
	str("PATH 2: RED");
	delay();
	
	red2 = 0;
	orange2 = 1;
	cmd(0x01);
	cmd(0x80);
	str("PATH 1: RED");
	cmd(0xc0);
	str("PATH 2: ORANGE");
	delay2();
	break;
}
}
}

// delay using TIMER 0  (1 second delay)
void delay0()
{
	// for 1 second delay we have to iterate upto 76 using prescaler = 256
/*
	Fout = F clk/ ( 4 * prescaler *(256-TMR0)* count) 
	we have to find count , set F clk = 20MHz = 20000000 
	 prescaler = 256 , TMR0 = 0 , Fout = 1/ Tout, Tout = 1 (desired delay in sec)
	count = 20000000/(4 * 256 *256 *1) = 76.2 
 	set iteration upto 76  for 1 second delay
*/
	 int i;
  for(i=0;i<77;i++) {
	if(button1 == 1)
		button1state = 1;
	if(button2 == 1)
		button2state = 1;
    while(!T0IF);
    T0IF=0;
  }
}


// delay using TIMER 2 (2 second)
void delay2()
{
	//	for 2 sec delay we have to iterate upto 153
/*
	Fout = F clk/ ( 4 * prescaler *(PR2-TMR2)* postscaler * count) 
	we have to find count , set F clk = 20MHz = 20000000 
	prescaler = 16 ,PR2 = 255, TMR2 = 0 ,postscaler = 16
	Fout = 1/ Tout, Tout = 2 (desired delay in sec)
	count = 20000000/(4 * 16 *255 * 16 * 0.5) = 153.18 
 	set iteration upto 153  for 2 second delay
*/
	int i ;
	T2CON |= (1<<2); // TMR 2 ON
	for (i=0 ; i< 153 ; i++)
{
	if(button1 == 1)
		button1state = 1;
	if(button2 == 1)
		button2state = 1;
	while (!TMR2IF); // wait until TMR2 flag rise (PIR 1 register)
	TMR2IF = 0;
}
}

void cmd(unsigned char a)
{
	rs = 0;
	PORTA &= 0xF0;
	PORTA |= ((a>>4) & 0x0f );
	en = 1;
	lcd_delay();
	en = 0;
	lcd_delay();
	PORTA &= 0xf0;
	PORTA |= (a & 0x0f);
	en=1;
	lcd_delay();
	en=0;
	lcd_delay();
}

void data(unsigned char b)
{
	rs=1;
	PORTA &=0xf0;
	PORTA |= ((b>>4)& 0x0f);
	en = 1;
	lcd_delay();
	en = 0;
	lcd_delay();
	PORTA &= 0xf0;
	PORTA |=(b & 0x0f);
	en = 1;
	lcd_delay();
	en=0 ;
	lcd_delay();

}

void str(unsigned char *s)
{
	while(*s)
{
	data(*s++);
}
}

void lcd()
{
	cmd(0x02);
	cmd(0x28);
	cmd(0x0e);
	cmd(0x06);
	cmd(0x80);
	cmd(0x01);
}

void lcd_delay()
{
	unsigned int lcd_delay;
	for(lcd_delay = 0;lcd_delay <=1000;lcd_delay ++);
}

void delay()
{
	int i;
	int segment[7] = { 0x3f,0x06, 0x5b,0x4f,0x66,0x6d,0x7d};

	for (i = 6;i>=0;i--)
{
	PORTD = segment[i];
	delay0();
}
}