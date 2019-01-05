#include <xb_board.h>
#include "XB_GLCD_S1D15710.h"
#include "xb_Font_4x6.h"

#ifdef LCD_S1D15710_USE

TXB_GLCD_S1D15710::TXB_GLCD_S1D15710()
{
	BeginPaintCounter = 0;
	Screen.Width = LCD_S1D15710_WIDTH;
	Screen.Height = LCD_S1D15710_HEIGHT;
	Screen.ClippingRect = {0,0,LCD_S1D15710_WIDTH-1,LCD_S1D15710_HEIGHT-1 };
	LCD_Init();
	Pen.Pos.x = 0;
	Pen.Pos.y = 0;
	Pen.Color = 1;
	SetCurrFont(&Font_4x6);
	Font.Color = 1;
}

TXB_GLCD_S1D15710::~TXB_GLCD_S1D15710()
{
}

void TXB_GLCD_S1D15710::LCD_Init()
{
	uint32_t pg, i;
	LCD_S1D15710_INIT_PORT();
	reset();
	command_send(DISP_OSCL);
	command_send(DISP_BIAS);
	command_send(DISP_TYPE);
	command_send(DISP_COMMON);
	command_send(DISP_START_LINE);
#if CONTROLLER_S1D17710
	command_send(DISP_PWR_SET);
#else
	command_send(0x2B);
#endif
	command_send(DISP_V5ADJ);
	command_send(DISP_ELECTRCTRL);
	command_send(0xa1);
	command_send(DISP_ONOFF);
	command_send(0xA3);
	command_send(0xA2);
	command_send(__END);


	for (pg = 0; pg <= LCD_S1D15710_MAXPAGE; pg++)
	{
		LCD_S1D15710_RD_SET();
		LCD_S1D15710_WR_RESET();
		LCD_S1D15710_A0_RESET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE(DISP_PAGE_ADDR + pg);
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE(DISP_COL_ADDR_H);
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE(DISP_COL_ADDR_L);
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_WR_SET(); 
		LCD_S1D15710_A0_SET();

		LCD_S1D15710_DATA_PORT_OUTPUT();
		LCD_S1D15710_WR_RESET();
		LCD_S1D15710_RD_SET();
		LCD_S1D15710_A0_SET();
		for (i = 0; i <= LCD_S1D15710_WIDTH + 1; i++)
		{
			LCD_S1D15710_CS1_RESET();
			LCD_S1D15710_DATA_WRITE(0x00);
			LCD_S1D15710_CS1_SET();
		}
	}

	LCD_Clear();
}
void TXB_GLCD_S1D15710::LCD_BeginPaint()
{
	BeginPaintCounter++;
}

void TXB_GLCD_S1D15710::LCD_EndPaint()
{
	BeginPaintCounter--;
	if (BeginPaintCounter <= 0)
	{
		BeginPaintCounter = 0;
		DrawLCDBuffer();
	}
}

int32_t TXB_GLCD_S1D15710::LCD_GetLCDWidth()
{
	return LCD_S1D15710_WIDTH;
}

int32_t TXB_GLCD_S1D15710::LCD_GetLCDHeight()
{
	return LCD_S1D15710_HEIGHT;
}

void TXB_GLCD_S1D15710::DrawLCDBuffer()
{
	register uint32_t i;
	register uint32_t pg;
	register uint8_t *buf = LCDBuffer;

	for (pg = 0; pg<LCD_S1D15710_MAXPAGE; pg++)
	{
		LCD_S1D15710_RD_SET();
		LCD_S1D15710_WR_RESET(); 
		LCD_S1D15710_A0_RESET(); 
		LCD_S1D15710_CS1_RESET(); 
		LCD_S1D15710_DATA_WRITE(DISP_PAGE_ADDR + pg);
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE(DISP_COL_ADDR_H);
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE(DISP_COL_ADDR_L + 1);
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_WR_SET();
		LCD_S1D15710_A0_SET();

		LCD_S1D15710_DATA_PORT_OUTPUT();
		LCD_S1D15710_WR_RESET();
		LCD_S1D15710_RD_SET();
		LCD_S1D15710_A0_SET();
		for (i = 0; i<LCD_S1D15710_WIDTH; i++)
		{
			LCD_S1D15710_CS1_RESET();
			LCD_S1D15710_DATA_WRITE(*buf++);
			LCD_S1D15710_CS1_SET();
		}
	}
}

void TXB_GLCD_S1D15710::LCD_Backlight(_REG uint8_t value)
{
	if (value & 1) LCD_S1D15710_BL1_ON();
	else LCD_S1D15710_BL1_OFF();
	if (value & 2) LCD_S1D15710_BL2_ON();
	else LCD_S1D15710_BL2_OFF();
}

void TXB_GLCD_S1D15710::LCD_Clear(void)
{
	register uint8_t *buf = LCDBuffer;
	register uint32_t i;

	for (i = 0; i<SIZELCDBUFFER; i++) *(buf++) = 0;
	if (BeginPaintCounter == 0) DrawLCDBuffer();
}


void TXB_GLCD_S1D15710::LCD_PutPixel(_REG int32_t Ax, _REG int32_t Ay, _REG TColorIndx Acolor)
{
	if (BeginPaintCounter > 0)
	{
		register uint32_t pg, pgadr;
		register uint32_t b, mask;

		pg = Ay >> 3;//Ay/8;
		pgadr = LCD_S1D15710_BEGINPAGEADR[pg] + Ax;
		mask = 1 << (Ay - (pg << 3));

		b = LCDBuffer[pgadr];
		if ((Acolor & 1) == 0)
		{
			mask = ~mask;
			b &= mask;
		}
		else
		{
			b |= mask;
		}
		LCDBuffer[pgadr] = b;
	}
	else
	{
		register uint32_t pg, pgadr;
		register uint32_t b, lb, mask;

		pg = Ay >> 3;//Ay/8;
		pgadr = LCD_S1D15710_BEGINPAGEADR[pg] + Ax;
		mask = 1 << (Ay - (pg << 3));

		b = LCDBuffer[pgadr];
		lb = b;
		if ((Acolor & 1) == 0)
		{
			mask = ~mask;
			b &= mask;
		}
		else
		{
			b |= mask;
		}
		if (lb == b) return;
		LCDBuffer[pgadr] = b;

		Ax++;
		LCD_S1D15710_RD_SET();
		LCD_S1D15710_WR_RESET();
		LCD_S1D15710_A0_RESET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE(DISP_PAGE_ADDR + pg);
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE((DISP_COL_ADDR_H + (Ax >> 4)));
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE((DISP_COL_ADDR_L + (Ax & 0x0f)));
		LCD_S1D15710_CS1_SET();
		LCD_S1D15710_WR_SET();
		LCD_S1D15710_A0_SET();

		LCD_S1D15710_WR_RESET();
		LCD_S1D15710_RD_SET();
		LCD_S1D15710_A0_SET();

		LCD_S1D15710_CS1_RESET();
		LCD_S1D15710_DATA_WRITE(b);
		LCD_S1D15710_CS1_SET();
	}
}

TColorIndx TXB_GLCD_S1D15710::LCD_GetPixel(_REG int32_t Ax, _REG int32_t Ay)
{
	register uint32_t pg, pgadr;

	pg = Ay >> 3;
	pgadr = LCD_S1D15710_BEGINPAGEADR[pg] + Ax;
	return (LCDBuffer[pgadr] >> (Ay - (pg << 3)) & 1);
}

#endif