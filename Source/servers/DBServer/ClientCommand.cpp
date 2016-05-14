#include "DBServer_PCH.h"
#include "ClientCommand.h"
#include <sstream>

#ifdef WIN32 

#include "conio.h"
#else

int _kbhit()
{
    struct timeval tv;
    fd_set rdfs;  
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO( &rdfs );
    FD_SET ( STDIN_FILENO , &rdfs );
    select( STDIN_FILENO + 1 , &rdfs , NULL , NULL , &tv );
    return FD_ISSET( STDIN_FILENO , &rdfs );
}

int _getch()
{
    return getchar();
}

#endif


VectorCommand GameCommand::s_sCommands;

GameCommand::GameCommand()
:	m_nCommandCount( 0 )
{
}

GameCommand::~GameCommand()
{
}

void GameCommand::Init()
{
	//s_sCommands.push_back( KeyCommand("0"," ¶Ï¿ªÁ¬½Ó "		,	ClientNetHandler::SendRequestClose));
	//s_sCommands.push_back( KeyCommand("11"," µÇÂ¼hzdÕËºÅ "		,	ClientNetHandler::SendLogin));
	//s_sCommands.push_back( KeyCommand("22"," µÇÂ¼kfcÕËºÅ "		,	ClientNetHandler::SendLogin2));

}

void GameCommand::Command( float32 nDelay )
{

	if( _kbhit() )
	{
		int32 nKey = _getch();
		switch( nKey )
		{
#ifdef WIN32
		case 13:			// Enter
#else
        case 10:			// Linux
#endif
			{
				if( m_nCommandCount == 0 )
					break;

				ParseCommand( m_arrBuffer );

				// reset command
				m_nCommandCount = 0;
				memset( m_arrBuffer , 0 , MAX_PATH1 );
				printf("\n");
			}
			break;
		case 8:				// BackSpace
			{
				if( m_nCommandCount > 0 )
				{
					printf( "\b \b" );
					m_arrBuffer[m_nCommandCount] = 0;
					m_nCommandCount--;
				}
			}
			break;
		case 27:			// ESC
			{
				// clear command
				m_nCommandCount = 0;
				memset( m_arrBuffer , 0 , MAX_PATH1 );
				printf( "...[Cancel]\nCommand: " );
			}
			break;
		default:
			{
				if( m_nCommandCount >= 255 )
				{
					break;
				}
				m_arrBuffer[ m_nCommandCount ] = (char)nKey;
				m_arrBuffer[ m_nCommandCount + 1 ] = '\0';
				m_nCommandCount++;
				printf( "%c", nKey );
			}
			break;
		}
	}
}

bool GameCommand::ParseCommand( char* command )
{
	VectorCommandIter iterEnd = s_sCommands.end();
	for ( VectorCommandIter iter = s_sCommands.begin() ; iter != iterEnd ; ++iter )
	{
		if ( strstr( command , iter->id.c_str() ) )
		{
// TODO			iter->callBack(this);
			return true;
		}
	}
	printf( "\nUnknown command.\n" );
	return false;
}

void		GameCommand::CommandHelp()
{
	printf("\n");
	VectorCommandIter iterEnd = s_sCommands.end();
	for ( VectorCommandIter iter = s_sCommands.begin() ; iter != iterEnd ; ++iter )
	{
		 printf( "%-8s  %-3s: %-20s\n" ,  "==========" , iter->id.c_str() ,iter->doc.c_str() );
	}
}

