#include "LoginScene.h"

//#include "KeyLogin.h"

LoginScene::LoginScene(FK2DEngine::CFKWindow& rWindow):mrWindow(rWindow), mWelcomeFont( rWindow.Graphics(), L"Arial", 20, 0 )
{

	std::wstring szFileName = L"";

	// 背景 
	szFileName = FK2DEngine::ShareResourcePrefix() + L"avgRc/bg_login_ui.jpg";
	m_pBackgroundImage.reset( new FK2DEngine::CImage( mrWindow.Graphics(), szFileName, false ) );


	mAccountLable.Create( SSGUIManager::Instance()->GetRootCtrl(),236, 356, L"帐号 >" );
	mPasswordLable.Create( SSGUIManager::Instance()->GetRootCtrl(),236, 396, L"密码 >" );


	// 登录框架 
	szFileName = FK2DEngine::ShareResourcePrefix() + L"rc\\media\\edit.bmp";
	mAccountBgImg = LoadImageFile( szFileName );
	mAccountBgCtrl.Create( SSGUIManager::Instance()->GetRootCtrl(), 290, 348, mAccountBgImg );

	m_pInputFont.reset( new FK2DEngine::CFont( mrWindow.Graphics(), L"Sans MT 936_S60", 18 ) );

	m_pTextFields[0].reset( new FTextInput( mrWindow, *m_pInputFont, 313, 357 ) );
	m_pTextFields[1].reset( new FTextInput( mrWindow, *m_pInputFont, 313, 397 ) );

	// 开动计时器
	m_Timer.Play();

	// 密码框架 
	szFileName = FK2DEngine::ShareResourcePrefix() + L"rc\\media\\edit.bmp";
	mPasswordBgImg = LoadImageFile( szFileName );
	mPasswordBgCtrl.Create( SSGUIManager::Instance()->GetRootCtrl(), 290, 388, mAccountBgImg );
	mPasswordEdit.Create( SSGUIManager::Instance()->GetRootCtrl(), 314, 416, 152 );
	//mPasswordEdit.SetFocus();

	szFileName = FK2DEngine::ShareResourcePrefix() + L"rc\\media\\PicButton.bmp";
	m_UIButtonImage = LoadImageFile( szFileName );
	CColor* m_pColor = (new CColor( CColor::WHITE ));
	m_Button.SetFontColor( *m_pColor );
	m_Button.Create( SSGUIManager::Instance()->GetRootCtrl(),(mrWindow.GetScreenWidth() - m_UIButtonImage.Width() / 4 ) / 2 , 436, m_UIButtonImage, L"进入游戏" );


	m_Button.OnMouseUp.invoker = this;
	m_Button.OnMouseUp.function = (MouseFun)(&LoginScene::LoginBtnPress);

	CArchiveFactory* pSystemArchiveFactory = new CFileSystemArchiveFactory();
	SSArchiveManager::Instance()->AddArchiveFactory( pSystemArchiveFactory );
	CFileSystemArchive *pFileSystem = static_cast< CFileSystemArchive *>( SSArchiveManager::Instance()->Load( FK2DEngine::ShareResourcePrefix() + L"rc", L"FileSystem"));
	StringVectorPtr pStrVectorFileSystem = pFileSystem->List(true);

	// 背景音乐

	m_pDataStream = pFileSystem->Open( L"music/bg_login_玉满堂.mp3" );
	unsigned int unSize = m_pDataStream.GetPointer()->Size();
	m_pSoundBuf = new char[ unSize ];		// 注: 该Buf不可释放，应当在退出程序时释放。
	m_pDataStream.GetPointer()->Read( static_cast< void* >( m_pSoundBuf ), unSize );
	m_pSongBuffer = new FK2DEngine::CBuffer( m_pSoundBuf, unSize );
	m_pReader = new CReader( *m_pSongBuffer, unSize );
	m_pReader->SetPosition( 0 );
	m_pSong.reset( new FK2DEngine::CSong( *m_pReader ));
	delete m_pSongBuffer;

	mAccountLable.SetZPos(1100);
	mPasswordLable.SetZPos(1100);
	mAccountBgCtrl.SetZPos(1100);
	mAccountEdit.SetZPos(1101);
	mPasswordBgCtrl.SetZPos(1100);
	mPasswordEdit.SetZPos(1101);
	m_Button.SetZPos(1100);

}

LoginScene::~LoginScene(void)
{
}

void LoginScene::DrawFinisih()
{
	m_pSong->Play( true );
}

void LoginScene::FKDraw() const 
{
	m_pBackgroundImage->FKDraw( 0, 0, 0 );

	FK2DEngine::CColor quadColor(0,0,0);
	mrWindow.Graphics().DrawQuad(
		0,		480,		quadColor,
		0,		600,		quadColor,
		800,	480,		quadColor,
		800,	600,		quadColor,  3
		);

	//for( unsigned int i = 0; i < LengthOf( m_pTextFields ); ++i )
	//{
	//	m_pTextFields[i]->FKDraw( 1 );
	//}

	mWelcomeFont.FKDraw( L"欢迎来到仙灵岛，这里就是传说中神仙聚会的地方，现在由赵灵儿为您带路吧...", 60, 500, 6, 1, 1, FK2DEngine::CColor(0,252,0) );

}

void LoginScene::LoginBtnPress(POINT pt, ENUM_MouseButton mb)
{
	std::wstring username = m_pTextFields[0]->Text();
	std::wstring password = m_pTextFields[1]->Text();


	// 检查，用户名，进行socket 连接 



}

void LoginScene::KeyDown( FK2DEngine::CKey p_Btn )
{
	if( p_Btn == FK2DEngine::EKB_Tab )
	{
		int nIndex = -1;
		for( unsigned int i = 0; i < LengthOf( m_pTextFields ); ++i )
		{
			if( mrWindow.Input().TextInput() == m_pTextFields[i].get() )
			{
				nIndex = i;
			}
		}

		mrWindow.Input().SetTextInput( m_pTextFields[(nIndex + 1) % LengthOf( m_pTextFields )].get() );
	}
	else if( p_Btn == FK2DEngine::EKB_Escape )
	{
		if( mrWindow.Input().TextInput() )
		{
			mrWindow.Input().SetTextInput( NULL );
		}
		else
		{
			mrWindow.Close();
		}
	}
	else if( p_Btn == FK2DEngine::MS_Left )
	{
		mrWindow.Input().SetTextInput( NULL );

		for( unsigned int i = 0; i < LengthOf(m_pTextFields); ++i )
		{
			if( m_pTextFields[i]->IsUnderPoint( mrWindow.Input().MouseX(), mrWindow.Input().MouseY() ) )
			{
				mrWindow.Input().SetTextInput( m_pTextFields[i].get() );
			}
		}
	}
}
