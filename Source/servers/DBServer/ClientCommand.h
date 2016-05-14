#ifndef _GAMECOMMAND_H_
#define _GAMECOMMAND_H_

#include "BaseDefine.h"

#define MAX_PATH1 260

class GameCommand;

typedef void(GameCommand::*DBCommandFun)();

struct KeyCommand
{
	KeyCommand( const char* id , const char* doc , DBCommandFun call )
	{
		this->id = id;
		this->doc = doc;
		this->callBack = call;
	}

	std::string	id;
	std::string	doc;
	DBCommandFun	callBack;
};

typedef vector< KeyCommand > VectorCommand;
typedef vector< KeyCommand >::iterator VectorCommandIter;

class GameCommand
{
public:

	GameCommand();
	~GameCommand();
    
    void Init();
	void Command( float32 delay );
	bool ParseCommand( char* command );
	void CommandHelp();

private:

	static VectorCommand	s_sCommands;
	char			m_arrBuffer[ MAX_PATH1 ];
	int32			m_nCommandCount;
};



#endif


