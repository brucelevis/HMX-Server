#ifndef _GAMECOMMAND_H_
#define _GAMECOMMAND_H_

#define MAX_PATH1 260

#include "../DyNetSocket/NetSocket.h"

class ClientPlayer;

typedef void(*CommandFun)(ClientPlayer*);

struct KeyCommand
{
	KeyCommand( const char* id , const char* doc , CommandFun call )
	{
		this->id = id;
		this->doc = doc;
		this->callBack = call;
	}

	string		id;
	string		doc;

	CommandFun	callBack;
};

typedef vector< KeyCommand > VectorCommand;
typedef vector< KeyCommand >::iterator VectorCommandIter;

class GameCommand
{
public:

	GameCommand();
	~GameCommand();
    
    void Init(NetSocket& net_socket,ClientPlayer*	gPlayer);

	void Command( float32 delay );
	bool ParseCommand( char* command );
	void CommandHelp();

private:

	static NetSocket*		mCommandSocket;
	static VectorCommand	s_sCommands;
	ClientPlayer*	m_cClientPlayer;
	char			m_arrBuffer[ MAX_PATH1 ];
	int32			m_nCommandCount;
};



#endif


