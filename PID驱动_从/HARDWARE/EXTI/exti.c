#include "exti.h"
#include "delay.h" 
#include "led.h" 
#include "adc.h"
#include "control.h"
#include "lcd.h"
#include "pid.h"
#include <string.h>
//////////////////////////////////////////////////////////////////////////////////	 
//�����ж�����
//EXTI2����E2����׼Դ����
//EXTI3����E3���Լ��������
////////////////////////////////////////////////////////////////////////////////// 
extern u8 choose,Update,SetFreq;
extern float Tin,Tout;
float Tdif;
extern HEDIT *edit1,*edit2,*edit3,*edit4,*edit5,*edit6;
extern float pwm;
extern float SetVol;
extern float PhaseShift,FreqWord,Phase;
extern float integral2,integral3;
void EXTI_GPIO_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//??GPIOA,GPIOE??
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //KEY0 KEY1 KEY2????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//??????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//??
	GPIO_Init(GPIOE, &GPIO_InitStructure);//???GPIOE4
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //KEY0 KEY1 KEY2????
	GPIO_Init(GPIOE, &GPIO_InitStructure);//???GPIOE4
}
void TIM_Int_Init()//TIM5 ��ʱ����Ƶ42MHz �������������ж�ʱ���
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  ///ʹ��TIM5ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  ///ʹ��TIM9ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);  ///ʹ��TIM10ʱ��
	
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=41;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=0; 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//��ʼ��TIM5
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);//��ʼ��TIM9
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseInitStructure);//��ʼ��TIM10
	
	TIM_Cmd(TIM5,ENABLE); //ʹ�ܶ�ʱ��5
	TIM_Cmd(TIM9,ENABLE); //ʹ�ܶ�ʱ��9
	TIM_Cmd(TIM10,ENABLE); //ʹ�ܶ�ʱ��10
}
void EXTI2_IRQHandler(void)//��׼Դ�����ж�
{
	TIM_SetCounter(TIM5,0);
	Tin=TIM_GetCounter(TIM9);
	TIM_SetCounter(TIM9,0);
	EXTI_ClearITPendingBit(EXTI_Line2);  //??LINE3???????  
}
void EXTI3_IRQHandler(void)//�Լ���������ж�
{
	Tdif=TIM_GetCounter(TIM5);
		//PID(-Tdif, 0, &FreqWord,&integral2,-3000,-10000,10000,5, 0.2, 0, 10000, 3000);
		//PID(Tdif, 0, &FreqWord,&integral3, 10000, -10000, 5000, 0, 0.1, 0, 500, -500);
	//Tout=TIM_GetCounter(TIM10);
	//TIM_SetCounter(TIM10,0);
	Tout=(200000000.0F/FreqWord);
		PID(-Tout,-Tin,&FreqWord,&integral2,-30000,-100000,1000000,0.5, 0.02, 0, 10000, 3000);
	//if (Tdif>FreqWord/5) //�������ڵ�һ�룬�ͽ�ʱ���ĳɸ���
	//	Tdif-=2*FreqWord/5;
	//PID(-Tdif, 0, &FreqWord,&integral2,-3000,-10000,10000,0, 5, 0, 10000, 3000);
	//PhaseShift=PhaseShift-Tdif/500000.0*FreqWord;
		//PID(Tdif, 0, &PhaseShift,&integral3, 10000, -10000, 5000, 0, 0.1, 0, 500, -500);
	//PhaseShift=PhaseShift-Tdif/5000000.0*FreqWord;
	EXTI_ClearITPendingBit(EXTI_Line3);//??LINE2??????? 
}


//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
	//NVIC_InitTypeDef   NVIC_InitStructure;
	//EXTI_InitTypeDef   EXTI_InitStructure;
	
	EXTI_GPIO_Init(); //������Ӧ��IO�ڳ�ʼ��
	TIM_Int_Init();
	/*RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	
	
	//����EXTI_Line2,3,4
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
	EXTI_Init(&EXTI_InitStructure);//����
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
	EXTI_Init(&EXTI_InitStructure);//����
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//�ⲿ�ж�2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����*/
	
	   
}












