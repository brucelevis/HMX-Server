#pragma once
#include "FKEngineInclude.h"
#include "boost/scoped_ptr.hpp"

#include "Config.h"
#include "FTextInput.h"

//--------------------------------------------------------------------


class LoginScene : public FK2DEngine::IGUIControl
{
private:

	static const unsigned long		SELECTION_COLOR = 0x00000000;

	boost::scoped_ptr< FK2DEngine::CImage >			m_pBackgroundImage;
	boost::scoped_ptr< FK2DEngine::CSong >			m_pSong;
	FK2DEngine::CBuffer*							m_pSongBuffer;
	FK2DEngine::CReader*							m_pReader;
	FK2DEngine::DataStreamPtr						m_pDataStream;
	char*											m_pSoundBuf;

	FK2DEngine::CFont								mWelcomeFont;


	boost::scoped_ptr< FK2DEngine::CFont >			m_pInputFont;
	boost::scoped_ptr< FK2DEngine::CImage >			m_pCursor;
	FK2DEngine::CTimer								m_Timer;

	FK2DEngine::CLable								mAccountLable;
	FK2DEngine::CLable								mPasswordLable;

	FK2DEngine::CImageControl						mAccountBgCtrl;
	FK2DEngine::CBitmap								mAccountBgImg;
	FK2DEngine::CEdit								mAccountEdit;
	boost::scoped_ptr< FTextInput >					m_pTextFields[2];

	FK2DEngine::CImageControl						mPasswordBgCtrl;
	FK2DEngine::CBitmap								mPasswordBgImg;
	FK2DEngine::CEdit								mPasswordEdit;

	FK2DEngine::CBitmap								m_UIButtonImage;
	FK2DEngine::CButton								m_Button;

	SMouseEvent										m_sEvent;

private:

	void DrawFinisih();

private:

	FK2DEngine::CFKWindow& mrWindow;

public:

	LoginScene(FK2DEngine::CFKWindow& rWindow);
	~LoginScene(void);

	virtual void FKDraw() const;
	void KeyDown( FK2DEngine::CKey p_Btn );

	void LoginBtnPress(POINT pt, ENUM_MouseButton mb);

};
