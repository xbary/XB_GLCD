#include <xb_board.h>
#include "XB_GLCD_SSD1306.h"
#include "xb_Font_4x6.h"

#ifdef LCD_SSD1306_USE

TXB_GLCD_SSD1306::TXB_GLCD_SSD1306()
{
	SSD1306_PIN_INIT();
	_SPISettings = SPISettings(4E6, MSBFIRST, SPI_MODE3);
	SPI.begin(SSD1306_D0_SCK_PIN, SSD1306_NONUSE_MISO_PIN, SSD1306_D1_MOSI_PIN, SSD1306_CS_PIN);

	BeginPaintCounter = 0;
	Screen.Width = LCD_SSD1306_WIDTH;
	Screen.Height = LCD_SSD1306_HEIGHT;
	Screen.ClippingRect = {0,0,LCD_SSD1306_WIDTH-1,LCD_SSD1306_HEIGHT-1 };

	LCD_Init();
	Pen.Pos.x = 0;
	Pen.Pos.y = 0;
	Pen.Color = 1;
	SetCurrFont(&Font_4x6);
	Font.Color = 1;
}

TXB_GLCD_SSD1306::~TXB_GLCD_SSD1306()
{
	SPI.end();
}


void TXB_GLCD_SSD1306::reset()
{
	SSD1306_RESET_PIN_SET_HIGH();
	delay(1);
	SSD1306_RESET_PIN_SET_LOW();
	delay(10);
	SSD1306_RESET_PIN_SET_HIGH();
	delay(1);
}

void TXB_GLCD_SSD1306::spi_beginTransaction()
{
	
	SPI.beginTransaction(_SPISettings);
	SSD1306_CS_PIN_SET_LOW();
	
}

void TXB_GLCD_SSD1306::spi_send_byte(uint8_t Abyte)
{
	SPI.transfer(Abyte);
}

void TXB_GLCD_SSD1306::spi_endTransaction()
{
	SSD1306_CS_PIN_SET_HIGH();
	SPI.endTransaction();
	
}

void TXB_GLCD_SSD1306::command(uint8_t Acommand)
{
	SSD1306_CS_PIN_SET_HIGH();
	SSD1306_DC_PIN_SET_LOW();
	spi_beginTransaction();
	spi_send_byte(Acommand);
	spi_endTransaction();
}

void TXB_GLCD_SSD1306::data(uint8_t Adata)
{
	SSD1306_CS_PIN_SET_HIGH();
	SSD1306_DC_PIN_SET_HIGH();
	spi_beginTransaction();
	spi_send_byte(Adata);
	spi_endTransaction();
}


void TXB_GLCD_SSD1306::LCD_Init()
{
	uint8_t vccstate = SSD1306_SWITCHCAPVCC;

	
	reset();

	command(SSD1306_DISPLAYOFF);                    // 0xAE
	command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	command(0x80);                                  // the suggested ratio 0x80
	command(SSD1306_SETMULTIPLEX);                  // 0xA8
	command(0x3F);
	command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	command(0x0);                                   // no offset
	command(SSD1306_SETSTARTLINE | 0x0);            // line #0
	command(SSD1306_CHARGEPUMP);                    // 0x8D
	if (vccstate == SSD1306_EXTERNALVCC) 
	{
		command(0x10);
	}
	else 
	{
		command(0x14);
	}
	command(SSD1306_MEMORYMODE);                    // 0x20
	command(0x00);                                  // 0x0 act like ks0108
	command(SSD1306_SEGREMAP | 0x1);
	command(SSD1306_COMSCANDEC);
	command(SSD1306_SETCOMPINS);                    // 0xDA
	command(0x12);
	command(SSD1306_SETCONTRAST);                   // 0x81
	if (vccstate == SSD1306_EXTERNALVCC) 
	{
		command(0x9F);
	}
	else 
	{
		command(0xCF);
	}
	command(SSD1306_SETPRECHARGE);                  // 0xd9
	if (vccstate == SSD1306_EXTERNALVCC) 
	{
		command(0x22);
	}
	else 
	{
		command(0xF1);
	}
	command(SSD1306_SETVCOMDETECT);                 // 0xDB
	command(0x40);
	command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	command(SSD1306_NORMALDISPLAY);                 // 0xA6
	
	command(SSD1306_DISPLAYON);//--turn on oled panel

	LCD_Clear();
}
void TXB_GLCD_SSD1306::LCD_BeginPaint()
{
	BeginPaintCounter++;
}

void TXB_GLCD_SSD1306::LCD_EndPaint()
{
	BeginPaintCounter--;
	if (BeginPaintCounter <= 0)
	{
		BeginPaintCounter = 0;
		DrawLCDBuffer();
	}
}

int32_t TXB_GLCD_SSD1306::LCD_GetLCDWidth()
{
	return LCD_SSD1306_WIDTH;
}

int32_t TXB_GLCD_SSD1306::LCD_GetLCDHeight()
{
	return LCD_SSD1306_HEIGHT;
}

void TXB_GLCD_SSD1306::DrawLCDBuffer()
{
	command(SSD1306_SEGREMAP | 0x1); // rotate screen 180
	command(SSD1306_COMSCANDEC); // rotate screen 180
	command(SSD1306_SETSTARTLINE | 0x0);            // line #0

	

	command(SSD1306_COLUMNADDR);
	command(0);   // Column start address (0 = reset)
	command(LCD_SSD1306_WIDTH - 1); // Column end address (127 = reset)
	command(SSD1306_PAGEADDR);
	command(0); // Page start address (0 = reset)

#if LCD_SSD1306_HEIGHT == 64
	command(7); // Page end address
#endif

#if LCD_SSD1306_HEIGHT == 32
	command(3); // Page end address
#endif

#if LCD_SSD1306_HEIGHT == 16
	command(1); // Page end address
#endif
	
		SSD1306_CS_PIN_SET_HIGH();
		SSD1306_DC_PIN_SET_HIGH();
		spi_beginTransaction();
		for (uint16_t i = 0; i < SIZELCDBUFFER; i++) 
		{
			spi_send_byte(LCDBuffer[i]);
		}
		spi_endTransaction();
}

void TXB_GLCD_SSD1306::LCD_Backlight(_REG uint8_t value)
{
	return;
}

void TXB_GLCD_SSD1306::LCD_Clear(void)
{
	register uint8_t *buf = LCDBuffer;
	register uint32_t i;

	for (i = 0; i<SIZELCDBUFFER; i++) *(buf++) = 0;
	if (BeginPaintCounter == 0) DrawLCDBuffer();
}

#define ssd1306_swap(a, b) {_REG int32_t t = a; a = b; b = t; }

void TXB_GLCD_SSD1306::LCD_PutPixel(_REG int32_t Ax, _REG int32_t Ay, _REG TColorIndx Acolor)
{
	if ((Ax < 0) || (Ax >= LCD_GetLCDWidth()) || (Ay < 0) || (Ay >= LCD_GetLCDHeight())) return;

	/*
	switch (rotation) {

	case 1:
	{
		ssd1306_swap(Ax, Ay);
		Ax = LCD_GetLCDWidth() - Ax - 1;
		break;
	}
	case 2:
	{
		Ax = LCD_GetLCDWidth() - Ax - 1;
		Ay = LCD_GetLCDHeight() - Ay - 1;
		break;
	}
	case 3:
	{
		ssd1306_swap(Ax, Ay);
		Ay = LCD_GetLCDHeight() - Ay - 1;
		break;
	}
	}
	*/
	// x is which column

	switch (Acolor) 
	{
	default:
	case 1:
	{
		LCDBuffer[Ax + (Ay / LCD_SSD1306_BITBYTE)*LCD_SSD1306_WIDTH] |= (1 << (Ay & 7));
		break;
	}
	case 0:
	{
		LCDBuffer[Ax + (Ay / LCD_SSD1306_BITBYTE)*LCD_SSD1306_WIDTH] &= ~(1 << (Ay & 7));
		break;
	}

//	case INVERSE:
//		buffer[x + (y / 8)*SSD1306_LCDWIDTH] ^= (1 << (y & 7));
//		break;
	}

	if (BeginPaintCounter == 0)
	{
		DrawLCDBuffer();
	}
}

TColorIndx TXB_GLCD_SSD1306::LCD_GetPixel(_REG int32_t Ax, _REG int32_t Ay)
{
	if (LCDBuffer[Ax + (Ay / LCD_SSD1306_BITBYTE)*LCD_SSD1306_WIDTH] & ~(1 << (Ay & 7)) == 0)
		return 0;
	else
		return 1;
}

#endif