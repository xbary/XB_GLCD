#ifndef XB_GLCD_S1D15710_H
#define XB_GLCD_S1D15710_H

#include <xb_board.h>
#include "XB_GLCD.h"

#define CONTROLLER_S1D17710 1							//S1D15710= 1, S1D15705= 0
#define LCD_S1D15710_WIDTH 219
#define LCD_S1D15710_HEIGHT 60
#define LCD_S1D15710_MAXPAGE 8
#define SIZELCDBUFFER (LCD_S1D15710_WIDTH*LCD_S1D15710_MAXPAGE)

#define MaxColumn  			0xDF						//maksymalna ilosæ kolumn

// *******************************************Commend makros***************************************
//(24)Built-in Oscillator Circuit ON
#define DISP_OSCL			0xAB						//0xAB= ON

//(11)LCD Bias Set
//Sets the LCD drive voltage bias ratio.
#define DISP_BIAS			0xA3						//A2= 1/9, A3= 1/7

//(9)Display Normal Rotation/Reversal
#define DISP_TYPE			0xA6						//0xA6= Negative, 0xA7= Normal

//(15)Common Output State Selection
#define DISP_COMMON			0xC8						//0xC8= Normal rotation, 0xC0= Reversal

//(2)Display start Line
#define DISP_START_LINE		0x7C						//01xx xxxx where xxxx= 0 to 63

//(16)Power Control Set
#define DISP_PWR_SET		0x2F						//0010 1xyz x= BOOST ON/OFF,y= V ON/OFF,z= VF ON/OFF

//(17)V5 Voltage Adjust Internal Resistance Ratio Set
#define DISP_V5ADJ			0x26						//0010 0xxx where xxx= 0 to 7

//(18)Electronic Control
// ********* 2-BAJT COMMAND ***********
#define DISP_ELECTRCTRL		0x81						//Contrast regulation by setting second byte.
//0xXX													//Electronic Control Register Set. Where XX= 0 -255

//(4)Column Address Set								    //Sets the column adress of the display RAM.
// **********2-BAJT COMMAND************
#define DISP_COL_ADDR_H		0x10						//0001 xxxx	 High order column adress.
#define DISP_COL_ADDR_L		0x00						//0000 xxxx	 Low order column adress.

//(1)Display ON/OFF
#define DISP_ONOFF			0xAF						//0xAF= ON, 0xAE= OFF

//(3)Page address Set								    //Sets the page address of the display RAM.
#define DISP_PAGE_ADDR		0xB0						//1011 xxxx where xxxx= 0 to 7

//(10)Display All Lighting On/Off						//High light all pixel on display
#define DISP_ALL_LIGHTING  	0xA4						//0xA4= normal display, 0xA5= all pixels ON

//(13) END												//Resets Read Modify Write
#define __END					0xEE

//(14) Reset											//Internal resetting
#define READ_MODIFY_WRITE	0xE0

class TXB_GLCD_S1D15710 : public TXB_GLCD
{
public:
	TXB_GLCD_S1D15710();
	virtual ~TXB_GLCD_S1D15710();
private:
	void LCD_Init();
	void LCD_Backlight(_REG uint8_t value);
	void LCD_Clear();
	void LCD_PutPixel(_REG int32_t Ax, _REG int32_t Ay, _REG TColorIndx Acolor);
	TColorIndx LCD_GetPixel(_REG int32_t Ax, _REG int32_t Ay);
	void LCD_BeginPaint();
	void LCD_EndPaint();
	int32_t LCD_GetLCDWidth();
	int32_t LCD_GetLCDHeight();
protected:
	int32_t BeginPaintCounter;
	uint8_t LCDBuffer[SIZELCDBUFFER];

	const uint32_t LCD_S1D15710_BEGINPAGEADR[LCD_S1D15710_MAXPAGE] = {
		0 * LCD_S1D15710_WIDTH,
		1 * LCD_S1D15710_WIDTH,
		2 * LCD_S1D15710_WIDTH,
		3 * LCD_S1D15710_WIDTH,
		4 * LCD_S1D15710_WIDTH,
		5 * LCD_S1D15710_WIDTH,
		6 * LCD_S1D15710_WIDTH,
		7 * LCD_S1D15710_WIDTH };

	inline void data_write(void) { LCD_S1D15710_DATA_PORT_OUTPUT(); }
	inline void data_read(void)	{ LCD_S1D15710_DATA_PORT_INPUT(); }
	inline void data_set(register uint8_t data8) { LCD_S1D15710_DATA_WRITE(data8); }
	inline uint8_t data_get() { return LCD_S1D15710_DATA_READ(); }
	inline void delay_cycles(register uint32_t delay) 
	{
		register uint32_t temp = 0;
		for (temp = 0; temp < delay; ++temp)
		{
			asm ("nop"); 
		}
	}
	inline void reset()
	{
		LCD_S1D15710_RES_RESET();
		LCD_S1D15710_RES_SET();
	}
	inline void command_send(unsigned char d)
	{
		LCD_S1D15710_RD_SET();
		LCD_S1D15710_WR_RESET();
		LCD_S1D15710_A0_RESET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE(d);
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_WR_SET();
		LCD_S1D15710_A0_SET();
	}
	inline void data_send(unsigned char ch)
	{
		LCD_S1D15710_WR_RESET(); 
		LCD_S1D15710_RD_SET(); 
		LCD_S1D15710_A0_SET(); 
		LCD_S1D15710_CS1_RESET(); 
		LCD_S1D15710_DATA_WRITE(ch);
		LCD_S1D15710_CS1_SET(); 
		LCD_S1D15710_WR_SET();
		LCD_S1D15710_A0_SET();
	}
	inline uint8_t dataport_read(void)
	{
		uint8_t dane;
		LCD_S1D15710_RES_SET();  
		LCD_S1D15710_WR_SET(); 
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_CS1_SET();
		data_set(0xAA);
		LCD_S1D15710_DATA_PORT_INPUT();
		LCD_S1D15710_RD_RESET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_CS1_SET();
		delay_cycles(15);
		LCD_S1D15710_CS1_RESET();
		asm ("nop");
		asm ("nop");
		dane = data_get();
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_RD_SET();
		LCD_S1D15710_DATA_PORT_OUTPUT();
		return dane;
	}

	void DrawLCDBuffer();
};

#endif