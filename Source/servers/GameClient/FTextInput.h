#ifndef __TEXT_FIELD_H_
#define __TEXT_FIELD_H_


#include "FKEngineInclude.h"
#include <cwctype>
#include <algorithm>


class FTextInput : public FK2DEngine::CTextInput
{
private:
	FK2DEngine::CFKWindow&			m_pWindow;
	FK2DEngine::CFont&				m_pFont;
	double							m_dX;
	double							m_dY;
	DWORD							m_dLastTime;
	double							m_zPos;
public:
	static const unsigned long		INACTIVE_COLOR	= 0xcc666666;
	static const unsigned long		ACTIVE_COLOR    = 0xccff6666;
	static const unsigned long		SELECTION_COLOR = 0xcc0000ff;
	static const unsigned long		CARET_COLOR     = 0xffffffff;
	static const int				PADDING			= 1;
public:
	FTextInput( FK2DEngine::CFKWindow& p_Window, FK2DEngine::CFont& p_Font, double p_dX, double p_dY )
		: m_pWindow( p_Window )
		, m_pFont( p_Font )
		, m_dX( p_dX )
		, m_dY( p_dY )
		, m_dLastTime( 0 ),m_zPos(1120)
	{
		SetText( L"Çëµã»÷..." );
	}

	void SetzPos(double zPos)
	{
		m_zPos = zPos;
	}

	static wchar_t ToUpper( wchar_t p_szIn )
	{
		using namespace std;
		return towupper( (wint_t)p_szIn );
	}

	std::wstring Filter( const std::wstring& p_szString ) const
	{
		std::wstring Result;
		Result.resize( p_szString.length() );
		std::transform( p_szString.begin(), p_szString.end(), Result.begin(), ToUpper );
		return Result;
	}

	void FKDraw( DWORD p_dwElapseTime ) const
	{
		FK2DEngine::CColor BackgroundColor;
		if( m_pWindow.Input().TextInput() == this )
		{
			BackgroundColor = ACTIVE_COLOR;
		}
		else
		{
			BackgroundColor = INACTIVE_COLOR;
		}

		m_pWindow.Graphics().DrawQuad(
			m_dX - PADDING,				m_dY - PADDING,				BackgroundColor,
			m_dX + Width() + PADDING,	m_dY - PADDING,				BackgroundColor,
			m_dX - PADDING,				m_dY + Height() + PADDING,	BackgroundColor,
			m_dX + Width() + PADDING,	m_dY + Height() + PADDING,	BackgroundColor, m_zPos - 1
			);

		double dPosX = m_dX + m_pFont.TextWidth( Text().substr( 0, CarePos() ) );
		double dSelX = m_dX + m_pFont.TextWidth( Text().substr( 0, SelectionStart() ) );

		m_pWindow.Graphics().DrawQuad(
			dSelX,		m_dY,			SELECTION_COLOR,
			dPosX,		m_dY,			SELECTION_COLOR,
			dSelX,		m_dY + Height(),SELECTION_COLOR,
			dPosX,		m_dY + Height(),SELECTION_COLOR, m_zPos
			);

		if( m_pWindow.Input().TextInput() == this )
		{
			//if(( p_dwElapseTime - m_dLastTime > 1000 ) || ( p_dwElapseTime - m_dLastTime < 500 ))
			//{
			//	if( p_dwElapseTime - m_dLastTime > 1000 )
			//	{
			//		m_dLastTime = p_dwElapseTime;
			//	}
				m_pWindow.Graphics().DrawLine( dPosX, m_dY,	CARET_COLOR, dPosX, m_dY + Height(), CARET_COLOR, m_zPos + 2 );
			//}
		}

		m_pFont.FKDraw( Text(), m_dX, m_dY, m_zPos + 1 );
	}

	double Width() const
	{
		return m_pFont.TextWidth( Text() );
	}

	double Height() const
	{
		return m_pFont.Height();
	}

	bool IsUnderPoint( double p_dMouseX, double p_dMouseY )
	{
		return ( ( p_dMouseX > m_dX - PADDING ) 
			&& ( p_dMouseX < m_dX + Width() + PADDING )
			&& ( p_dMouseY > m_dY - PADDING )
			&& ( p_dMouseY < m_dY + Height() + PADDING ) );
	}
};
//--------------------------------------------------------------------

#endif