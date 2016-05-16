//#include "../DyNetSocket/NetClient.h"
#include "FKEngineInclude.h"
#include "boost/scoped_ptr.hpp"


//#include "Config.h"
#include "LoginScene.h"

//#include "NetMsgHandler.h"

//--------------------------------------------------------------------
#ifdef _DEBUG
#pragma comment(lib, "LibClientEngine.lib")
//#pragma comment(lib, "LIBSocket.lib")
#else
#pragma comment(lib, "LibClientEngine.lib")
#pragma comment(lib, "LIBSocket.lib")
#endif
//--------------------------------------------------------------------

class CGameWindow : public FK2DEngine::CFKWindow
{
	static const unsigned long		SELECTION_COLOR = 0x66000000;
private:
	boost::scoped_ptr< FK2DEngine::CImage >			m_pBackgroundImage;
	boost::scoped_ptr< FK2DEngine::CImage >			m_pNameTitleImage;
	FK2DEngine::CImage*								m_pBodyImage;
	boost::scoped_ptr< FK2DEngine::CImage >			m_pSmallHeadImage;
	boost::scoped_ptr< FK2DEngine::CSong >			m_pSong;
	FK2DEngine::CBitmap								m_pBitmap;
	FK2DEngine::CBuffer*							m_pSongBuffer;
	FK2DEngine::CReader*							m_pReader;
	FK2DEngine::CFont								m_NameFont;
	FK2DEngine::CFont								m_NameFontBack;
	FK2DEngine::CFont								m_Font;
	FK2DEngine::CFont								m_FontBack;
	FK2DEngine::DataStreamPtr						m_pDataStream;
	char*											m_pSoundBuf;

	LoginScene	loginScene;

public:
	CGameWindow()
		: CFKWindow( 800, 600, false )
		, m_NameFont( Graphics(), L"宋体", 28, FK2DEngine::EFF_None )
		, m_NameFontBack( Graphics(), L"宋体", 28, FK2DEngine::EFF_None )
		, m_Font( Graphics(), L"Arial", 25, 0 )
		, m_FontBack( Graphics(), L"Arial", 25, 0 ),loginScene(*this)
	{
 		SetCaption( L"RPG游戏Demo" );

		// 显示图片
		std::wstring szFileName = FK2DEngine::ShareResourcePrefix() + L"avgRc/bg_login_ui.jpg";
		m_pBackgroundImage.reset( new FK2DEngine::CImage( Graphics(), szFileName, false ) );

		// 设置鼠标
		szFileName = FK2DEngine::ShareResourcePrefix() + L"avgRc/Attack.cur";
		SetNewCursor( szFileName );

		SSGUIManager::Instance()->AddCtrl(&loginScene);
		
	}
public:
	void FKDraw()
	{
		loginScene.FKDraw();
	}

	bool Close()
	{
		return __super::Close();
	}

	void KeyDown( FK2DEngine::CKey p_Key )
	{
		if( p_Key == FK2DEngine::EKB_Escape )
		{
			Close();
		}
		else if( p_Key == FK2DEngine::MS_WheelUp )
		{
			//m_pSong->ChangeVolume( m_pSong->Volume() - 0.05 );
		}
		else if( p_Key == FK2DEngine::MS_WheelDown )
		{
			//m_pSong->ChangeVolume( m_pSong->Volume() + 0.05 );
		}
		/*else if( p_Key == FK2DEngine::EKB_Space )
		{
 			if( m_pSong->Playing() )
			{
				m_pSong->Pause();
			}
			else if( m_pSong->Paused() )
			{
				m_pSong->Play( true );
			}
		}*/
		else if( p_Key == FK2DEngine::EKB_PrintScreen )
		{
			PrintScreen( FK2DEngine::ShareResourcePrefix() + L"Screen\\" + ToString( GetTime() ) + L".bmp" );
		}

		loginScene.KeyDown(p_Key);
		
	}
};
//--------------------------------------------------------------------
int main( int p_nArgc, char* p_Argv[] )
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1574);

	//NetClient* gNetClient = new NetClient();
	//gNetClient->SetAddress("127.0.0.1", 9900);
	//gNetClient->SetHandler(OnNetMsgEnter, OnNetMsg, OnNetMsgExit);
	//gNetClient->Start();

	try
	{
		CGameWindow window;
		window.Show();
	}
	catch( ... )
	{
		throw;
	}

	_CrtDumpMemoryLeaks();
}
//--------------------------------------------------------------------
