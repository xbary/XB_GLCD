#ifndef XB_GLCD_H
#define XB_GLCD_H

#include <xb_board.h>

#define _REG register
typedef uint8_t TColorIndx;

// Typ opisuj¹cy kolor RGB 
typedef union {
	uint32_t raw;
	struct {
		uint8_t Z;
		uint8_t R;
		uint8_t G;
		uint8_t B;
	} c;
} TColorRGB;

typedef struct
{
	int32_t x;
	int32_t y;
} TPos;

typedef struct
{
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
} TRect;

typedef struct
{
	int32_t x1;
	int32_t y1;
	int32_t x2;
	int32_t y2;
} TRectPos;



typedef struct
{
	int32_t Width;
	int32_t Height;
	TRect ClippingRect;
} TScreen;


typedef struct
{
	TPos Pos; // Aktualna pozycja pióra
	TPos Move; // Przesuniêcie po ka¿dej operacji PutPixel i GetPixel
	TColorIndx Color; // Aktualny kolor

} TPen;

// Struktura opisuj¹ definicje kszta³tów fontów
typedef struct
{
	const uint8_t Width; 		// Szerokosc fonta , jesli stala szerokosc, jeœli ró¿na to 0
	const uint8_t Height; 		// Wyoskoœæ fonta
	const uint8_t BytesPerWidth; 	// Iloœæ bajtów na szerokoœæ fonta, potrzebne do wyliczenia offestu we fontach o tym samym rozmiarze
	const uint8_t Flags;            // Flagi definicji fontów, 
									// bit 0 = 0 bajt opisuje bitmape w poziomie
									// bit 0 = 1 bajt opisuje bitmape w pionie
	const uint8_t BitPerPixel; 	// Ilosc bitow na piksel .. uszykowane pod kolorowe fonty
	const uint8_t BeginFontIndx;	// numer pierwszego zdefiniowanego fonta
	const uint8_t EndFontIndx;	// numer ostatniego zdefiniowanego fonta
	const uint8_t TransparenTColorIndxIndx; // Indeks koloru w definicjach kszta³tów koloru przeŸroczystego
	uint8_t *BitmapFonts; 		// wskaŸnik bitmapy z fontami
	uint8_t *WidthFonts;   		// Tablica z szerokoci¹ fontów, jesli NULL to sta³a szerkosæ Width
	uint16_t *OffsetFont;		// Tablica z ofsetami odktórych zaczynaj¹ siê kszta³ty fontów w bitmapie
} TFontDef;

typedef struct
{
	TFontDef *FontDef;
	TColorIndx Color;
} TFont;

typedef uint8_t TDrawPixelsFlags;

#define dpfTransparent	1
#define dpfChangeColor	2

typedef struct
{
	uint8_t *PixPtr;
	uint32_t Start;
	uint32_t Stop;
	uint8_t BitPerPixel;
	uint8_t MovX;
	uint8_t MovY;
	TColorIndx TransparenTColorIndx;
	TColorIndx *ChangeColorPtr;
	TDrawPixelsFlags Flags;
} TDrawPixelsDef;

typedef uint32_t TTextFlags;

#define tfDefault    	0
#define tfLeftJustify 	1
#define tfHCenter    	2
#define tfRightJustify 	4
#define tfTop		 	8
#define tfVCenter	 	16
#define tfBottom	 	32
#define tfWrap		 	64
#define tfWordWrap	 	128
#define tfBorderLeft    256
#define tfBorderTop		512
#define tfBorderRight   1024
#define tfBorderBottom  2048
#define tfBorder        (tfBorderLeft | tfBorderTop | tfBorderRight | tfBorderBottom)

#define tfSpacing1x     4096
#define tfSpacing2x     8192
#define tfSpacing3x     (tfSpacing1x | tfSpacing2x)


class TXB_GLCD
{
public:
	TXB_GLCD();
	~TXB_GLCD();
	TScreen Screen;
	TPen Pen;
	TFont Font;

	TColorRGB *PaletteColor;
	uint8_t SizePalette;

	void Backlight(_REG uint8_t value);
	void Clear(void);
	void SetPenPos(_REG int32_t Ax, _REG int32_t Ay);
	void SetPenMove(_REG int32_t Ax, _REG int32_t Ay);
	TColorIndx SetPenColor(_REG TColorIndx AColor);
	void DoPenMove(void);
	void PutPixel(void);
	TColorIndx GetPixel(void);
	void LineTo(_REG int32_t x2, _REG int32_t y2);
	void Rect(_REG TRect *r);
	void FillRect(_REG TRect *r);
	void SetCurrFont(const TFontDef *Afd);
	void BeginPaint(void);
	void EndPaint(void);
	void DrawText(_REG const char *AString);
	void DrawText(_REG char *AString);
	void DrawChar(_REG uint8_t ch);
	void DrawPixels(_REG TDrawPixelsDef *DPD);
	void SetRectMask(_REG TRect *Ar, _REG TRect *Alastmask);
	int32_t CharWidth(_REG char Ach);
	int32_t TextWidth(_REG char *Atext, _REG uint8_t Aendch);
	int32_t TextHeight(_REG char *Atext);
	void TextToTextWrap(_REG char *AText, _REG char *ATextWrap, _REG uint32_t sizebuftextwrap, _REG int32_t AWidthRect, _REG TTextFlags ATextFlags);
	void RectCorrect(_REG TRect *Ars, _REG TRect *Ard);
	void TextRect(char *Atext, uint8_t Aendch, TRect *Arect, TTextFlags ATextFlags);
private:
	virtual void LCD_Init();
	virtual void LCD_Backlight(_REG uint8_t value);
	virtual void LCD_Clear();
	virtual void LCD_PutPixel(_REG int32_t Ax, _REG int32_t Ay, _REG TColorIndx Acolor);
	virtual TColorIndx LCD_GetPixel(_REG int32_t Ax, _REG int32_t Ay);
	virtual void LCD_BeginPaint();
	virtual void LCD_EndPaint();
	virtual int32_t LCD_GetLCDWidth();
	virtual int32_t LCD_GetLCDHeight();


};

#endif