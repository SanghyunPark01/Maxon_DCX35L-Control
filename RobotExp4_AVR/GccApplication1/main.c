/*
 * GccApplication1.c
 *
 * Created: 2019-08-28 오후 12:13:16
 * Author : CDSL
 */ 

// Inculde //
#include "mcu_init.h"
#include "dataType.h"
#include <math.h>

// define //
// motor parameter
#define Ra 1.76
#define La 0.000658
#define Kt 0.0683
#define Ke 0.0683
// time
#define dt_cur 0.0005
#define dt_vel 0.005
#define dt_pos 0.05
// etc..
#define RAD2DEG 57.295779
#define DEG2RAD 0.0174532

// Gain 
// Position Control 
#define Kp_p 2
#define Kd_p 0.8
// Speed Control       
#define Kp_s 1.505
#define Ki_s 41.658
#define Ka_s 0.96445
// Current Control
#define Kp_c 0.827
#define Ki_c 2.2117e+03
#define Ka_c 4.030632809

// variables //
// Encoder
volatile int32_t g_Cnt = 0;
volatile int32_t g_preCnt = 0;

// Serial Signal
volatile Packet_t g_PacketBuffer;
volatile unsigned char checkSize;
volatile unsigned char g_buf[256], g_BufWriteCnt, g_BufReadCnt;
volatile unsigned char g_PacketMode;
volatile unsigned char g_ID = 0;
// g_ID == 1 Mobile mode
// g_ID == 2 Joint mode
  
// Position Control
volatile double g_Pdes = 0.;
volatile double g_Ppre = 0.;
volatile double g_Pcur = 0.;
volatile double g_Perr = 0.;
volatile double g_Pperr = 0.;
volatile double g_pos_control;

// Speed Control
volatile double g_Vdes = 0.;
volatile double g_Vcur = 0.;
volatile double g_Verr = 0.;
volatile double g_Verr_sum = 0.;
volatile double g_Vlimit = 10.;
volatile double g_vel_control;

// Current Control
volatile double g_Cdes = 0.;
volatile double g_Ccur = 0.;
volatile double g_Cerr = 0.;
volatile double g_Cerr_sum = 0;
volatile double g_Climit = 1.;
volatile int cur_control = 0;

// etc
volatile double g_ADC;	// Current Sensor Value
volatile int g_SendFlag = 0;
volatile unsigned char g_TimerCnt; //Timer Cnt

// mode
volatile int mode = 0;

// Init
volatile int Set = 0;


void SetDutyCW(double v)
{
	while(TCNT1  == 0);

	int ocr = v * (200. / 24.) + 200;
	
	if(ocr > OCR_MAX)	ocr = OCR_MAX;
	else if(ocr < OCR_MIN)	ocr = OCR_MIN;
	//OCR1A = OCR1B = ocr;
	
	OCR1A = OCR3B = ocr + 8;		//1 H
	OCR1B = OCR3A = ocr - 8;		//1 L
}

void InitLS7366()
{
	PORTB = 0x00;
	SPI_MasterSend(SELECT_MDR0 | WR_REG);
	SPI_MasterSend(X4_QUAD | FREE_RUN | DISABLE_INDEX | SYNCHRONOUS_INDEX |FILTER_CDF_1);
	PORTB = 0x01;
	
	PORTB = 0x00;
	SPI_MasterSend(SELECT_MDR1 | WR_REG);
	SPI_MasterSend(FOUR_BYTE_COUNT_MODE | ENABLE_COUNTING);
	PORTB = 0x01;
	
	PORTB = 0x00;
	SPI_MasterSend(SELECT_CNTR | CLR_REG);
	PORTB = 0x01;
}

int getADC(char ch)
{
	ADMUX = (ADMUX & 0xf0) + ch;
	ADCSRA |= 0x40;
	while(!(ADCSRA & 0x10));
	return ADC;
}

ISR(USART0_RX_vect)
{
	g_buf[g_BufWriteCnt++] = UDR0;
}

// AVR내에서 Mobile, Joint mode 구현
//ISR(INT0_vect)
//{
	//// Mobile mode = Angle Limit X
	//mode = 1; 
//}
//ISR(INT1_vect)
//{
	//// Joint mode = Angle Limit 0 ~ 360
	//mode = 2;
//}

// Set test용 스위치 인터럽트
ISR(INT0_vect)
{
	Set = 1;
}
ISR(INT1_vect)
{
	Set = 2;
}

///////////////////
// Control Start //
ISR(TIMER0_OVF_vect)
{	
	TCNT0 = 256 - 125;
	//Read LS7366
	int32_t cnt;
	
	PORTC = 0x01;
	
	g_ADC = getADC(0);
	
	PORTB = 0x00;
	SPI_MasterSend(SELECT_OTR | LOAD_REG);
	PORTB = 0x01;
			
	PORTB = 0x00;
	SPI_MasterSend(SELECT_OTR | RD_REG);
	cnt = SPI_MasterRecv();		cnt = cnt<< 8;
	cnt |= SPI_MasterRecv();	cnt = cnt<< 8;
	cnt |= SPI_MasterRecv();	cnt = cnt<< 8;
	cnt |= SPI_MasterRecv();
	PORTB = 0x01;
	g_Cnt = -cnt;
	
	g_Pcur = (g_Cnt / (4096. * 81.)) * 2 * M_PI;
	
	PORTC = 0x03;
	
	//TO DO
	
	
	//////////////////////
	// Position Control //
	if((g_TimerCnt % 100) == 0)
	{
		g_TimerCnt = 0;
		
		// Init Set
		//if(Set == 1)
		//{
			//g_Pdes = 90 * DEG2RAD;
		//}
		//else if(Set == 2)
		//{
			//g_Pdes = 0;
		//}
		
		// Packet ID를 통한 예외처리
		// Mobile mode
		//if(mode == 1)
		//{
			//g_Pdes = 450 * DEG2RAD;
		//}
		
		// Joint mode
		if(mode == 2)
		{
			if(g_Pdes >= 360 * DEG2RAD)
			{
				g_Pdes = fmod(g_Pdes, 360 * DEG2RAD);
				//g_Pdes -= 360 * DEG2RAD;
			}
			else if(g_Pdes < 0)
			{
				g_Pdes = (-fmod(-g_Pdes, 360 * DEG2RAD) + 360 * DEG2RAD);
				//g_Pdes += 360 * DEG2RAD;
			}
		}
		// 회전 방향 전환
		//g_Pdes = -1*g_Pdes;
		
		// D제어를 위한 previous Error 저장
		g_Pperr = g_Perr;
		
		// Error 저장
		g_Perr = g_Pdes - g_Pcur;
		
		// Position Control output = Speed value
		g_pos_control = Kp_p * g_Perr + Kd_p * ((g_Perr - g_Pperr) / dt_pos);
		
		// Saturation
		if(g_pos_control > 642.65/81.) g_pos_control = 642.65/81.;
		else if(g_pos_control < -642.65/81.) g_pos_control = -642.65/81.;
	}
	
	
	///////////////////
	// Speed Control //
	if((g_TimerCnt % 10) == 0)
	{
		// Reference
		g_Vdes = g_pos_control;
		//g_Vdes = 1.5;
		
		// MFC Saturation
		if(g_Vdes > g_Vlimit) g_Vdes = g_Vlimit;
		else if(g_Vdes < -g_Vlimit) g_Vdes = - g_Vlimit;
		
		// 현재 속도, 이전 속도 저장
		g_Vcur = (g_Pcur - g_Ppre) / 0.005;
		g_Ppre = g_Pcur;
		
		// Error, Error sum 저장
		g_Verr = g_Vdes - g_Vcur;
		g_Verr_sum += g_Verr;
		
		// Speed Control output = Current value
		g_vel_control = Kp_s * g_Verr + Ki_s * g_Verr_sum * dt_vel;
		
		// Current Saturation & Anti wind-up
		if(g_vel_control > 2.08)
		{
			g_Verr_sum -= (g_vel_control - 2.08) * Ka_s;
			g_vel_control = 2.08;
		}
		else if(g_vel_control < -2.08)
		{
			g_Verr_sum -= (g_vel_control + 2.08) * Ka_s;
			g_vel_control = -2.08;
		}
		
	}
	
	
	/////////////////////
	// Current Control //
	
	// Reference
	g_Cdes = g_vel_control;
	
	// MFC Saturation
	if(g_Cdes > g_Climit) g_Cdes = g_Climit;
	else if(g_Cdes < - g_Climit) g_Cdes = -g_Climit;
	
	// 현재 전류값 저장
	g_Ccur = -( ((g_ADC / 1024. * 5.) - 2.5) * 10.);
	
	// Error, Error sum 저장
	g_Cerr = g_Cdes - g_Ccur;
	g_Cerr_sum += g_Cerr;
	
	// Current Control output = Voltage value;
	cur_control = Kp_c * g_Cerr + Ki_c * g_Cerr_sum * dt_cur;
	
	// EMF feed-forward
	cur_control += g_Vcur * Ke;
	
	// Voltage Saturation & Anti wind-up
	if(cur_control > 24)
	{ 
		g_Cerr_sum -= (cur_control - 24) * Ka_c;
		cur_control = 24;
	}
	else if(cur_control < -24)
	{
		g_Cerr_sum -= (cur_control + 24) * Ka_c;
		cur_control = -24;
	}
	
	SetDutyCW(cur_control);

///////////////////
// Control End //
	g_TimerCnt++;
	g_SendFlag++;

}



int main(void)
{
	Packet_t packet;
	packet.data.header[0] = packet.data.header[1] = packet.data.header[2] = packet.data.header[3] = 0xFE;
	
	InitIO();
	InitExtInt();
	
	//Uart
	InitUart0();
	InitUart1();
	
	//SPI
	InitSPI();
	
	//Timer
	InitTimer0();
	InitTimer1();
	InitTimer3();


	TCNT1 = TCNT3 = 0;
	SetDutyCW(0.);
	
	//ADC
	InitADC();
	
	//LS7366
	InitLS7366();
	
	TCNT0 = 256 - 125;
	sei();

	unsigned char check = 0;
	
    while (1) 
	{
		//////////
		// test //
		//TransNumUart1(g_Ccur*1000);
		//TransUart1(' ');
		//TransUart1('/');
		//TransUart1(' ');
		//TransNumUart1(cur_control);
		//TransNumUart1(g_Cnt);
		
		//TransNumUart1(g_Vdes * 100);
		//TransUart1(' ');
		//TransUart1('/');
		//TransUart1(' ');
		//TransNumUart1(g_Vcur * 100);
		//TransUart1(' ');
		//TransUart1('/');
		//TransUart1(' ');
		//TransNumUart1(g_Cdes * 100);
		//TransUart1(' ');
		//TransUart1('/');
		//TransUart1(' ');
		//TransNumUart1(g_Ccur * 100);
		//TransUart1(13);
		
		
		for(;g_BufReadCnt != g_BufWriteCnt; g_BufReadCnt++)
		{
			
			switch(g_PacketMode)
			{
			case 0:
				
				if (g_buf[g_BufReadCnt] == 0xFF) 
				{
					checkSize++;
					if (checkSize == 4) 
					{
						g_PacketMode = 1;
					}
				}
				else 
				{
					checkSize = 0;
				}
				break;
				
			case 1:

				g_PacketBuffer.buffer[checkSize++] = g_buf[g_BufReadCnt];
				
				if (checkSize == 8) 
				{
					//if(g_PacketBuffer.data.id == g_ID)
					//{
						//g_PacketMode = 2;
					//}
					
					if(g_PacketBuffer.data.id == 1)
					{
						mode = 1;	// Mobile mode
						g_PacketMode = 2;
					}
					else if(g_PacketBuffer.data.id == 2)
					{
						mode = 2;	// Joint mode
						g_PacketMode = 2;
					}
					else
					{
						g_PacketMode = 0;
						checkSize = 0;
					}
				}

				break;
			
			case 2:
				
				g_PacketBuffer.buffer[checkSize++] = g_buf[g_BufReadCnt];
				check += g_buf[g_BufReadCnt];
				
				if (checkSize == g_PacketBuffer.data.size) 
				{

					if(check == g_PacketBuffer.data.check)
					{

						switch(g_PacketBuffer.data.mode)
						{

							case 2:
							g_Pdes = g_PacketBuffer.data.pos / 1000.;
							//if(mode == 1) 
							//{
								//g_Pdes = g_PacketBuffer.data.pos / 1000.;
							//}
							//else if(mode == 2)
							//{
								//g_Pdes = (g_PacketBuffer.data.pos / 1000.);
								//if(g_Pdes >= 360)
								//{
									//g_Pdes -= 360;	
								//}
								//else if(g_Pdes < 0)
								//{
									//g_Pdes += 360; 
								//}
							//}
							g_Vlimit = g_PacketBuffer.data.velo / 1000.;
							g_Climit = g_PacketBuffer.data.cur / 1000.;
							break;
							}
					}
					
					check = 0;
					g_PacketMode = 0;
					checkSize = 0;
				}
				else if(checkSize > g_PacketBuffer.data.size || checkSize > sizeof(Packet_t)) 
				{
					TransUart0('f');
					check = 0;
					g_PacketMode = 0;
					checkSize = 0;
				}
			}
		}

		if(g_SendFlag > 19)
		{
			g_SendFlag = 0;			

			packet.data.id = g_ID;
			packet.data.size = sizeof(Packet_data_t);
			packet.data.mode = 3;
			packet.data.check = 0;
			
			//packet.data.pos = g_Pdes * 1000; 
			//packet.data.velo = g_Vlimit * 1000;
			//packet.data.cur = g_Climit * 1000;  
			
			// 원래
			packet.data.pos = g_Pcur * 1000;
			packet.data.velo = g_Vcur * 1000;
			packet.data.cur = g_Ccur * 1000;
			
			// test
			//packet.data.pos = -1 * g_Pcur * 1000;
			//packet.data.velo = -1 * g_Vcur * 1000;
			//packet.data.cur = -1 * g_Ccur * 1000;
			
			for (int i = 8; i < sizeof(Packet_t); i++)
			packet.data.check += packet.buffer[i];
			
			for(int i=0; i<packet.data.size; i++)
			{
				TransUart0(packet.buffer[i]);
			}
		}
	}
}