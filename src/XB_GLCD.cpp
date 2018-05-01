#include <xb_board.h>
#include "XB_GLCD.h"


TXB_GLCD::TXB_GLCD()
{

}


TXB_GLCD::~TXB_GLCD()
{


}

// ---------------------------------------------------------------------------------------------------------------------------
// Otwarcie biblioteki z u¿ycie wskazanego drivera
// argumenty
//	  -> Driver = adres w pamiêci flash na strukture drivera LCD
//	  -> AGLCD_Library = adres struktury otwatej biblioteki, jeœli NULL funkcja sama przydzieli
// rezultat
//    <- wskaŸnik na otwart¹ biblioteke
// rezultat dzia³ania
//    - rezerwacja ramu na zmienne biblioteki
//    - ustawienie wskaŸnika globalnego na aktualnie ob³ugiwan¹ biblioteke
// 	  - inicjowanie drivera LCD
/*TGLCD_Library *GLCD_OpenLibrary(TGLCD_Driver *Driver, TGLCD_Library *AGLCD_Library)
{
	if (Driver == NULL) return NULL;
	_REG uint32_t i = sizeof(TGLCD_Library);
	_REG TGLCD_Library *LCD = AGLCD_Library;

	if (LCD == NULL) LCD = xbmalloc(i);
	if (LCD == NULL) return NULL;
	//	do
	//{
	//((uint8_t *)LCD)[--i] = 0;
	//} while (i > 0);

	GLCD_UseLibrary(LCD);

	LCD->CurDrv = Driver;
	LCD->CurDrv->Init();

	LCD->Screen.Width = Driver->GetLCDWidth();
	LCD->Screen.Height = Driver->GetLCDHeight();

	LCD->Font.FontDef = NULL;

	GLCD_SetPenPos(0, 0);
	GLCD_SetPenMove(0, 0);
	GLCD_SetPenColor(1);

	GLCD_SetRectMask(NULL, NULL);

	GLCD_Backlight(1);

	return LCD;
}
// ---------------------------------------------------------------------------------------------------------------------------
// Zamkniêcie biblioteki
// argumenty
//	  -> library = biblioteka otwarta wczeniej
// rezultat
//
// rezultat dzia³ania
//    - nast¹pi deinicjacja LCD
//	  - zgaszenie podwietlania
// 	  - zwolnienie pamiêci zmiennych biblioteki
void GLCD_CloseLibrary(TGLCD_Library *Library)
{
	GLCD_Backlight(0);
	xbfree(Library);
}
// ---------------------------------------------------------------------------------------------------------------------------
// UStawienie adresu aktualnej biblioteki
void GLCD_UseLibrary(_REG TGLCD_Library *Library)
{
	GLCD_Current_Library = Library;
}
*/
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::Backlight(_REG uint8_t value)
{
	LCD_Backlight(value);
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::Clear(void)
{
	SetRectMask(NULL, NULL);
	SetPenColor(0);
	LCD_Clear();
	SetPenPos(0, 0);
	SetPenColor(1);
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::SetPenPos(_REG int32_t Ax, _REG int32_t Ay)
{
	Pen.Pos.x = Ax;
	Pen.Pos.y = Ay;
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::SetPenMove(_REG int32_t Ax, _REG int32_t Ay)
{
	Pen.Move.x = Ax;
	Pen.Move.y = Ay;
}
// ---------------------------------------------------------------------------------------------------------------------------
TColorIndx TXB_GLCD::SetPenColor(_REG TColorIndx AColor)
{
	_REG TColorIndx lc = Pen.Color;
	Pen.Color = AColor;
	return lc;
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::DoPenMove(void)
{
	Pen.Pos.x += Pen.Move.x;
	Pen.Pos.y += Pen.Move.y;
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::PutPixel(void)
{
	_REG int32_t x = Pen.Pos.x, y = Pen.Pos.y;

	if (x >= Screen.ClippingRect.left)
		if (x <= Screen.ClippingRect.right)
			if (y >= Screen.ClippingRect.top)
				if (y <= Screen.ClippingRect.bottom)
				{
					LCD_PutPixel(x, y, Pen.Color);
				}
	DoPenMove();
}
// ---------------------------------------------------------------------------------------------------------------------------
TColorIndx TXB_GLCD::GetPixel(void)
{
	_REG int32_t x = Pen.Pos.x, y = Pen.Pos.y;

	if (x >= Screen.ClippingRect.left)
		if (x <= Screen.ClippingRect.right)
			if (y >= Screen.ClippingRect.top)
				if (y <= Screen.ClippingRect.bottom)
				{
					return LCD_GetPixel(x, y);
				}
	return 0;
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::LineTo(_REG int32_t x2, _REG int32_t y2)
{
	_REG int32_t x1 = Pen.Pos.x;
	_REG int32_t y1 = Pen.Pos.y;
	_REG int32_t tx = x2;
	_REG int32_t ty = y2;

	LCD_BeginPaint();
	SetPenMove(0, 0);
	_REG int32_t x, y;
	_REG int32_t dx;
	_REG int32_t dy;

	if (y1 == y2)
	{
		if (x1 > x2)
		{
			x = x2;
			x2 = x1;
			x1 = x;
		}
		else
			x = x1;

		SetPenMove(1, 0);
		SetPenPos(x, y1);
		while (x <= x2)
		{
			PutPixel();
			x++;
		}
	}
	else
	{
		if (y1 > y2)
		{
			dy = y1 - y2;
			Pen.Move.y = -1;
		}
		else
		{
			dy = y2 - y1;
			Pen.Move.y = 1;
		}

		if (x1 == x2)
		{
			if (y1 > y2)
			{
				y = y2;
				y2 = y1;
				y1 = y;
			}
			else
				y = y1;

			SetPenMove(0, 1);
			SetPenPos(x1, y);
			while (y <= y2)
			{
				PutPixel();
				y++;
			}
		}
		else
		{

			if (x1 > x2)
			{
				dx = x1 - x2;
				Pen.Move.x = -1;
				x = x2;
				x2 = x1;
				y = y2;
				y2 = y1;
			}
			else
			{
				dx = x2 - x1;
				Pen.Move.x = 1;
				x = x1;
				y = y1;
			}

			if (dx == dy)
			{
				SetPenPos(x1, y1);
				while (dx-- >= 0)
				{
					PutPixel();
				}
			}
			else
			{

				SetPenPos(x, y);
				PutPixel();
				if (y < y2)
				{
					if (dx > dy)
					{
						_REG int32_t p = dy * 2 - dx;
						_REG int32_t twoDy = 2 * dy;
						_REG int32_t twoDyMinusDx = 2 * (dy - dx);
						while (x < x2)
						{
							x++;
							if (p < 0)
							{
								p += twoDy;
							}
							else
							{
								y++;
								p += twoDyMinusDx;
							}
							SetPenPos(x, y);
							PutPixel();
						}
					}
					else
					{
						_REG int32_t p = dx * 2 - dy;
						_REG int32_t twoDx = 2 * dx;
						_REG int32_t twoDxMinusDy = 2 * (dx - dy);
						while (y < y2)
						{
							y++;
							if (p < 0)
							{
								p += twoDx;
							}
							else
							{
								x++;
								p += twoDxMinusDy;
							}
							SetPenPos(x, y);
							PutPixel();
						}
					}
				}
				else
				{
					if (dx > dy)
					{
						_REG int32_t p = dy * 2 - dx;
						_REG int32_t twoDy = 2 * dy;
						_REG int32_t twoDyMinusDx = 2 * (dy - dx);
						while (x < x2)
						{
							x++;
							if (p < 0)
							{
								p += twoDy;
							}
							else
							{
								y--;
								p += twoDyMinusDx;
							}
							SetPenPos(x, y);
							PutPixel();
						}
					}
					else
					{
						_REG int32_t p = dx * 2 - dy;
						_REG int32_t twoDx = 2 * dx;
						_REG int32_t twoDxMinusDy = 2 * (dx - dy);
						while (y2 < y)
						{
							y--;
							if (p < 0)
							{
								p += twoDx;
							}
							else
							{
								x++;
								p += twoDxMinusDy;
							}
							SetPenPos(x, y);
							PutPixel();
						}
					}
				}
			}
		}
	}
	SetPenMove(0, 0);
	SetPenPos(tx, ty);
	LCD_EndPaint();
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::Rect(_REG TRect *r)
{
	_REG int32_t x1 = r->left, y1 = r->top, x2 = r->right, y2 = r->bottom;
	SetPenMove(0, 0);
	SetPenPos(x1, y1);
	LineTo(x2, y1);
	LineTo(x2, y2);
	LineTo(x1, y2);
	LineTo(x1, y1);
	SetPenPos(x2, y2);
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::FillRect(_REG TRect *r)
{
	_REG int32_t x, y;

	SetPenMove(0, 1);

	for (x = r->left; x <= r->right; x++)
	{
		SetPenPos(x, r->top);
		for (y = r->top; y <= r->bottom; y++)
		{
			PutPixel();
		}
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::SetCurrFont(const TFontDef *Afd)
{
	Font.FontDef = (TFontDef *)Afd;
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::BeginPaint(void)
{
	LCD_BeginPaint();
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::EndPaint(void)
{
	LCD_EndPaint();
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::DrawText(_REG const char *AString)
{
	DrawText((char *)AString);
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::DrawText(_REG char *AString)
{
	_REG char ch;
	int32_t lx = Pen.Pos.x;
	while ((ch = *(AString++)))
	{
		switch (ch)
		{
		case 0x0a:
		{
			Pen.Pos.y += Font.FontDef->Height;
			break;
		}
		case 0x0d:
		{
			Pen.Pos.x = lx;
			break;
		}
		default:
		{
			DrawChar(ch);
		}
		}
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
//uint8_t DPmaske[]={0b00000000,0b10000000,0b11000000,0b11100000,0b11110000,0b11111000,0b11111100,0b11111110,0b11111111};
uint8_t DPmaske[] = { 0, 128, 192, 224, 240, 248, 252, 254, 255 };

void TXB_GLCD::DrawPixels(_REG TDrawPixelsDef *DPD)
{
	
	_REG int32_t mx = DPD->MovX, my = DPD->MovY, x = Pen.Pos.x, y = Pen.Pos.y;
	_REG int32_t rl = Screen.ClippingRect.left;
	_REG int32_t rr = Screen.ClippingRect.right;
	_REG int32_t rt = Screen.ClippingRect.top;
	_REG int32_t rb = Screen.ClippingRect.bottom;

	_REG int32_t indxpix;
	_REG uint32_t vi, vbi, vby;
	_REG TColorIndx v;

	_REG uint8_t *PixPtr = DPD->PixPtr;
	_REG uint8_t BitPerPixel = DPD->BitPerPixel;
	_REG uint32_t mask = DPmaske[BitPerPixel];

	vi = (DPD->Start*BitPerPixel);
	indxpix = DPD->Stop - DPD->Start;

	if (DPD->Flags & dpfChangeColor)
	{
		_REG TColorIndx *chc = DPD->ChangeColorPtr;

		if (DPD->Flags & dpfTransparent)
		{
			_REG TColorIndx trancolor = DPD->TransparenTColorIndx;
			for (; indxpix >= 0; indxpix--)
			{
				if (x >= rl)
					if (x <= rr)
						if (y >= rt)
							if (y <= rb)
							{
								vby = vi >> 3;
								vbi = vi - (vby << 3);
								mask = DPmaske[BitPerPixel];

								if ((vbi + BitPerPixel)<8)
								{
									v = (PixPtr[vby] & mask >> vbi) >> (8 - vbi - BitPerPixel);
								}
								else
								{
									v = (PixPtr[vby++] & mask >> vbi);
									vbi = BitPerPixel + vbi - 8;
									v = v << vbi;
									mask = DPmaske[vbi];
									v |= ((PixPtr[vby] & mask) >> (8 - vbi));
								}
								if (v != trancolor) LCD_PutPixel(x, y, chc[v]);
							}
				x += mx;
				y += my;
				vi += BitPerPixel;
			}
		}
		else
		{
			for (; indxpix >= 0; indxpix--)
			{
				if (x >= rl)
					if (x <= rr)
						if (y >= rt)
							if (y <= rb)
							{
								vby = vi >> 3;
								vbi = vi - (vby << 3);
								mask = DPmaske[BitPerPixel];

								if ((vbi + BitPerPixel)<8)
								{
									v = (PixPtr[vby] & mask >> vbi) >> (8 - vbi - BitPerPixel);
								}
								else
								{
									v = (PixPtr[vby++] & mask >> vbi);
									vbi = BitPerPixel + vbi - 8;
									v = v << vbi;
									mask = DPmaske[vbi];
									v |= ((PixPtr[vby] & mask) >> (8 - vbi));
								}

								LCD_PutPixel(x, y, chc[v]);
							}
				x += mx;
				y += my;
				vi += BitPerPixel;
			}
		}
	}
	else
	{
		if (DPD->Flags & dpfTransparent)
		{
			_REG TColorIndx trancolor = DPD->TransparenTColorIndx;
			for (; indxpix >= 0; indxpix--)
			{
				if (x >= rl)
					if (x <= rr)
						if (y >= rt)
							if (y <= rb)
							{
								vby = vi >> 3;
								vbi = vi - (vby << 3);
								mask = DPmaske[BitPerPixel];

								if ((vbi + BitPerPixel)<8)
								{
									v = (PixPtr[vby] & mask >> vbi) >> (8 - vbi - BitPerPixel);
								}
								else
								{
									v = (PixPtr[vby++] & mask >> vbi);
									vbi = BitPerPixel + vbi - 8;
									v = v << vbi;
									mask = DPmaske[vbi];
									v |= ((PixPtr[vby] & mask) >> (8 - vbi));
								}
								if (v != trancolor) LCD_PutPixel(x, y, v);
							}
				x += mx;
				y += my;
				vi += BitPerPixel;
			}
		}
		else
		{
			for (; indxpix >= 0; indxpix--)
			{
				if (x >= rl)
					if (x <= rr)
						if (y >= rt)
							if (y <= rb)
							{
								vby = vi >> 3;
								vbi = vi - (vby << 3);
								mask = DPmaske[BitPerPixel];

								if ((vbi + BitPerPixel)<8)
								{
									v = (PixPtr[vby] & mask >> vbi) >> (8 - vbi - BitPerPixel);
								}
								else
								{
									v = (PixPtr[vby++] & mask >> vbi);
									vbi = BitPerPixel + vbi - 8;
									v = v << vbi;
									mask = DPmaske[vbi];
									v |= ((PixPtr[vby] & mask) >> (8 - vbi));
								}

								LCD_PutPixel(x, y, v);
							}
				x += mx;
				y += my;
				vi += BitPerPixel;
			}
		}
	}
	Pen.Pos.x = x;
	Pen.Pos.y = y;
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::DrawChar(_REG uint8_t ch)
{
	_REG TFontDef *FD = Font.FontDef;
	if (FD == NULL) return;

	TColorIndx ColorIndx[2] = { 0,1 };
	TDrawPixelsDef DPD = { FD->BitmapFonts, 0, 0, FD->BitPerPixel, 0, 0, 0, ColorIndx, 0 };// dpfChangeColor

																										  //	TColorIndx chc[2] = { 0, 1 };
																										  //	DPD.ChangeColorPtr=(TColorIndx *)&chc;
																										  //	DPD.Flags=dpfChangeColor;
																										  //	DPD.PixPtr = FD->BitmapFonts;
																										  //	DPD.BitPerPixel = FD->BitPerPixel;;



	_REG uint32_t chi, bpw, wf, ih, h, y, x;

	if ((ch >= FD->BeginFontIndx) && (ch <= FD->EndFontIndx))
	{
		chi = ch - FD->BeginFontIndx;
		h = FD->Height;

		if (FD->WidthFonts != NULL)
		{
			wf = FD->WidthFonts[chi];
			if (wf == 0) return;

			if ((FD->Flags & 1) == 1)
			{
				bpw = ((h - 1) >> 3) + 1;
			}
			else
			{
				bpw = ((wf - 1) >> 3) + 1;
			}
		}
		else
		{
			wf = FD->Width;
			bpw = FD->BytesPerWidth;
			if (bpw == 0) return;
		}


		if (FD->OffsetFont != NULL)
		{
			DPD.PixPtr += FD->OffsetFont[chi];
		}
		else
		{
			DPD.PixPtr += (chi*(h*bpw));
		}


		DPD.Start = 0;
		//	DPD.Flags=0;

		if ((FD->Flags & 1) == 1)
		{
			y = Pen.Pos.y -= (h - 1);

			DPD.Stop = h - 1;

			DPD.MovX = 0;
			DPD.MovY = 1;

			for (ih = 0; ih<wf; ih++)
			{
				Pen.Pos.y = y;
				DrawPixels(&DPD);
				DPD.PixPtr += bpw;
				Pen.Pos.x++;
			}
			Pen.Pos.y--;
		}
		else
		{
			Pen.Pos.y -= (h);

			DPD.Stop = wf - 1;

			x = Pen.Pos.x;

			DPD.MovX = 1;
			DPD.MovY = 0;

			for (ih = 0; ih<h; ih++)
			{
				Pen.Pos.y++;
				Pen.Pos.x = x;
				DrawPixels(&DPD);
				DPD.PixPtr += bpw;
			}
		}
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::SetRectMask(_REG TRect *Ar, _REG TRect *Alastmask)
{

	if (Alastmask != NULL)
	{
		Alastmask->left = Screen.ClippingRect.left;
		Alastmask->top = Screen.ClippingRect.top;
		Alastmask->right = Screen.ClippingRect.right;
		Alastmask->bottom = Screen.ClippingRect.bottom;
	}

	if (Ar == NULL)
	{
		Screen.ClippingRect.left = 0;
		Screen.ClippingRect.top = 0;
		Screen.ClippingRect.right = Screen.Width - 1;
		Screen.ClippingRect.bottom = Screen.Height - 1;
	}
	else
	{
		Screen.ClippingRect.left = Ar->left;
		Screen.ClippingRect.top = Ar->top;
		Screen.ClippingRect.right = Ar->right;
		Screen.ClippingRect.bottom = Ar->bottom;
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
int32_t TXB_GLCD::CharWidth(_REG char Ach)
{
	_REG TFontDef *FD = Font.FontDef;
	if (FD == NULL) return 0;
	_REG uint8_t chi;
	if ((Ach >= FD->BeginFontIndx) && (Ach <= FD->EndFontIndx))
	{
		if (FD->WidthFonts != NULL)
		{
			chi = Ach - FD->BeginFontIndx;
			return FD->WidthFonts[chi];
		}
		else
		{
			return FD->Width;
		}
	}
	return 0;
}
// ---------------------------------------------------------------------------------------------------------------------------
int32_t TXB_GLCD::TextWidth(_REG char *Atext, _REG uint8_t Aendch)
{
	_REG TFontDef *FD = Font.FontDef;
	if (FD == NULL) return 0;

	_REG uint8_t ch = *(Atext++);
	_REG int32_t wt = 0, mwt = 0;

	while (ch != Aendch)
	{
		switch (ch)
		{
		case 0: return mwt;
		case 0x0a: break;
		case 0x0d:
		{
			wt = 0;
			break;
		}
		default:
		{
			wt += CharWidth(ch);
			break;
		}
		}
		if (wt>mwt) mwt = wt;
		ch = *(Atext++);
	}
	return mwt;
}
// ---------------------------------------------------------------------------------------------------------------------------
int32_t TXB_GLCD::TextHeight(_REG char *Atext)
{
	_REG TFontDef *FD = Font.FontDef;
	if (FD == NULL) return 0;

	_REG char ch;
	_REG int32_t h = FD->Height;

	while ((ch = *(Atext++)))
	{
		switch (ch)
		{
		case 0x0a:
		{
			h += FD->Height;
			break;
		}
		case 0x0d: break;
		default:
		{
			break;
		}
		}
	}
	return h;
}
// ---------------------------------------------------------------------------------------------------------------------------
void STR_Delete(_REG char *AText, _REG uint32_t Aindx, _REG uint32_t Acount)
{
	_REG char ch;
	while (1)
	{
		ch = AText[Aindx + Acount];
		AText[Aindx++] = ch;
		if (ch == 0) return;
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
void STR_CutCharOnChar(_REG char *AText, _REG char ACutChar, _REG char AOnChar, _REG char AIgnorChar)
{
	_REG int32_t indx = 0;
	_REG char ch;

	while (1)
	{
		ch = AText[indx];
		if (ch == 0) return;

		if (ch == AOnChar)
		{
			while (1)
			{
				indx--;
				if (indx<0)
				{
					indx = 0;
					break;
				}
				ch = AText[indx];
				if (ch != AIgnorChar)
				{
					if (ch == ACutChar)
					{
						STR_Delete(AText, indx, 1);
					}
					else break;
				}
			}

			while (1)
			{
				ch = AText[indx];
				if (ch == 0) return;
				if (ch == AOnChar) break;
				indx++;
			}

			indx++;
			while (1)
			{
				ch = AText[indx];
				if (ch == 0) return;
				if (ch != AIgnorChar)
				{
					if (ch == ACutChar)
					{
						STR_Delete(AText, indx, 1);
					}
					else break;
				}
				else indx++;
			}
		}
		indx++;
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
void STR_TrimLeftChar(_REG char *AText, _REG char ATrimChar)
{
	_REG uint32_t indx = 0, count = 0;
	_REG char ch;

	while (1)
	{
		ch = AText[indx];
		if (ch == 0) return;
		if (ch == ATrimChar) count++;
		else break;
		indx++;
	}
	if (count>0)
	{
		STR_Delete(AText, 0, count);
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
void STR_TrimRightChar(_REG char *AText, _REG char ATrimChar)
{
	_REG uint32_t indx = 0;
	_REG char ch;

	while (1)
	{
		ch = AText[indx];
		if (ch == 0) break;
		indx++;
	}
	indx--;
	while (1)
	{
		ch = AText[indx];
		if (ch == 0) return;
		if (ch == ATrimChar) AText[indx] = 0;
		else break;
		indx--;
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
void STR_TrimChar(_REG char *AText, _REG char ATrimChar)
{
	STR_TrimLeftChar(AText, ATrimChar);
	STR_TrimRightChar(AText, ATrimChar);
}
// ---------------------------------------------------------------------------------------------------------------------------
uint32_t STR_Length(_REG char *AText, _REG char AEndChar)
{
	_REG uint32_t indx = 0, count = 0;
	_REG char ch;

	while (1)
	{
		ch = AText[indx];
		if ((ch == 0) || (ch == AEndChar)) break;
		count++;
		indx++;
	}
	return count;
}
// ---------------------------------------------------------------------------------------------------------------------------
void STR_Trim(_REG char *AText)
{
	_REG uint32_t l, ll = STR_Length(AText, 0);
	while (1)
	{
		l = ll;
		STR_TrimChar(AText, 0x20);
		STR_TrimChar(AText, 0x0a);
		STR_TrimChar(AText, 0x0d);
		ll = STR_Length(AText, 0);
		if (ll == l) break;
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::TextToTextWrap(_REG char *AText, _REG char *ATextWrap, _REG uint32_t sizebuftextwrap, _REG int32_t AWidthRect, _REG TTextFlags ATextFlags)
{
	_REG uint8_t ch, wrap = 0;
	int32_t indx_dw = 0, indx_sw = 0, indx_s = 0, indx_d = 0, wt = 0;

	while (AText[indx_s] != 0)
	{
		ch = AText[indx_s];
		ATextWrap[indx_d] = ch;

		if (ATextFlags & tfWrap)
		{
			switch (ch)
			{
			case 0x0a: break;
			case 0x0d:
			{
				wt = 0;
				break;
			}
			default:
			{
				wt += CharWidth(ch);
				if (wt>AWidthRect)
				{
					wt = CharWidth(ch);
					ATextWrap[indx_d++] = 0x0a; if (indx_d >= sizebuftextwrap) 	break;
					ATextWrap[indx_d++] = 0x0d; if (indx_d >= sizebuftextwrap) 	break;
					ATextWrap[indx_d] = ch; if (indx_d >= sizebuftextwrap) 	break;
				}
				break;
			}
			}
		}
		else if (ATextFlags & tfWordWrap)
		{
			switch (ch)
			{
			case 0x0a: break;
			case 0x0d:
			{
				wt = 0;
				wrap = 0;
				break;
			}
			case 0x20:
			{
				wt += CharWidth(ch);
				if ((wt>AWidthRect) && (wrap == 0))
				{
					if ((indx_s>0) && (AText[indx_s - 1] == 0x20))
					{

					}
					else
					{
						ATextWrap[indx_d] = 0x0a; if (indx_d >= sizebuftextwrap) 	break;
						ATextWrap[++indx_d] = 0x0d; if (indx_d >= sizebuftextwrap) 	break;
						wrap--;
						wt = 0;
					}
				}
				indx_sw = indx_s;
				indx_dw = indx_d;
				wrap++;
				break;
			}
			default:
			{
				wt += CharWidth(ch);
				if (wt>AWidthRect)
				{
					if (wrap>0)
					{
						wt = 0;
						wrap = 0;

						indx_s = indx_sw;
						indx_d = indx_dw;

						ATextWrap[indx_d] = 0x0a; if (indx_d >= sizebuftextwrap) 	break;
						ATextWrap[++indx_d] = 0x0d; if (indx_d >= sizebuftextwrap) 	break;
					}

				};
				break;
			}
			}
		}
		else
		{
			if (ATextFlags & tfLeftJustify)
			{
				switch (ch)
				{
				case 0x0a: break;
				case 0x0d:
				{
					wt = 0;
					break;
				}
				default:
				{
					if (wt>AWidthRect)
					{
						indx_d--;
					}
					wt += CharWidth(ch);
					break;
				}
				}
			}
		}

		indx_s++;
		indx_d++;
		if (indx_d >= sizebuftextwrap)
		{
			indx_d--;
			break;
		}
	}
	ATextWrap[indx_d] = 0;

	if (ATextFlags & tfWordWrap)
	{
		STR_CutCharOnChar(ATextWrap, 0x20, 0x0d, 0x0a);
	}
	STR_Trim(ATextWrap);
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::RectCorrect(_REG TRect *Ars, _REG TRect *Ard)
{
	if (Ard == NULL)
	{
		_REG int32_t tmp;

		if (Ars->left>Ars->right)
		{
			tmp = Ars->left;
			Ars->left = Ars->right;
			Ars->right = tmp;
		}

		if (Ars->top>Ars->bottom)
		{
			tmp = Ars->top;
			Ars->top = Ars->bottom;
			Ars->bottom = tmp;
		}
	}
	else
	{
		if (Ars->left<Ars->right)
		{
			Ard->left = Ars->left;
			Ard->right = Ars->right;
		}
		else
		{
			Ard->left = Ars->right;
			Ard->right = Ars->left;
		}

		if (Ars->top<Ars->bottom)
		{
			Ard->top = Ars->top;
			Ard->bottom = Ars->bottom;
		}
		else
		{
			Ard->top = Ars->bottom;
			Ard->bottom = Ars->top;
		}
	}
}
// ---------------------------------------------------------------------------------------------------------------------------
void TXB_GLCD::TextRect(char *Atext, uint8_t Aendch, TRect *Arect, TTextFlags ATextFlags)
{
	_REG TFontDef *FD = Font.FontDef;
	if (FD == NULL) return;

	_REG char ch;
	_REG int32_t fh = FD->Height;
	_REG int32_t cx = 0, cy = 0, xw, yw, sx = 0, sy = 0;
	TRect rect, lastmask;

	uint32_t i;
	char txt[255];
	for (i = 0; i<255; i++) txt[i] = 0;

	if (Arect == NULL) Arect = &Screen.ClippingRect;
	RectCorrect(Arect, &rect);


	if (ATextFlags == tfDefault) ATextFlags = tfLeftJustify | tfTop | tfWrap;

	SetRectMask(&rect, &lastmask);

	if (ATextFlags & tfBorderLeft)
	{
		SetPenPos(rect.left, rect.bottom);
		LineTo(rect.left, rect.top);
		rect.left++;
	}
	else SetPenPos(rect.left, rect.top);

	if (ATextFlags & tfBorderTop)
	{
		LineTo(rect.right, rect.top);
		rect.top++;
	}
	else SetPenPos(rect.right, rect.top);


	if (ATextFlags & tfBorderRight)
	{
		LineTo(rect.right, rect.bottom);
		rect.right--;
	}
	else SetPenPos(rect.right, rect.bottom);


	if (ATextFlags & tfBorderBottom)
	{
		LineTo(rect.left, rect.bottom);
		rect.bottom--;
	}


	if (ATextFlags & tfSpacing1x)
	{
		rect.left += 1;
		rect.top += 1;
		rect.right -= 1;
		rect.bottom -= 1;
	}
	if (ATextFlags & tfSpacing2x)
	{
		rect.left += 2;
		rect.top += 2;
		rect.right -= 2;
		rect.bottom -= 2;
	}

	SetRectMask(&rect, NULL);

	xw = rect.right - rect.left;
	yw = rect.bottom - rect.top;

	if (xw>3 && yw>3)
	{
		TextToTextWrap(Atext, txt, 255, xw, ATextFlags);

		if (ATextFlags & tfTop)
		{
			cy = rect.top;
			sy = cy + fh - 1;
		}
		if (ATextFlags & tfVCenter)
		{
			cy = (yw >> 1) + rect.top;
			sy = (cy - (TextHeight(txt) >> 1)) + fh - 1;
		}
		if (ATextFlags & tfBottom)
		{
			cy = rect.bottom;
			sy = (cy - TextHeight(txt)) + fh;
		}


		if (ATextFlags & tfLeftJustify)
		{
			cx = rect.left;
			sx = cx;
		}

		if (ATextFlags & tfHCenter)
		{
			cx = (xw >> 1) + rect.left;
			sx = cx - (TextWidth(txt, 0x0d) >> 1);
		}

		if (ATextFlags & tfRightJustify)
		{
			cx = rect.right + 1;
			sx = cx - TextWidth(txt, 0x0d);
		}

		SetPenPos(sx, sy);

		i = 0;
		for (i = 0; i<255; i++)
		{
			ch = txt[i];
			if (ch == 0) break;

			switch (ch)
			{
			case 0x0a:
			{
				Pen.Pos.y += fh;
				break;
			}
			case 0x0d:
			{
				if (ATextFlags & tfLeftJustify)
					sx = cx;
				else if (ATextFlags & tfHCenter)
					sx = cx - (TextWidth(&txt[i + 1], 0x0d) >> 1);
				else if (ATextFlags & tfRightJustify)
					sx = cx - TextWidth(&txt[i + 1], 0x0d);

				Pen.Pos.x = sx;
				break;
			}
			default:
			{
				DrawChar(ch);
			}
			}
		}
	}
	SetRectMask(&lastmask, NULL);
}
// ---------------------------------------------------------------------------------------------------------------------------
