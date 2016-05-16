#include "GameClient_PCH.h"
#include "ClientPlayer.h"
#include "Encrypt.h"

ClientPlayer::ClientPlayer(void)
{
	memcpy(m_arrEncryptKey,EncryptDefualt,MAX_ENCRYPT_LENTH);
	m_bChating = false;

}


ClientPlayer::~ClientPlayer(void)
{
}

void ClientPlayer::Update(int32 nSrvTimes)
{
	if(m_bChating)
	{
		int32 nRand = mSocket->SID() % 20;
		int32 nSeconds = nSrvTimes % 20;
		if(nSeconds == nRand )
		{
			C2SCharWorld sMsg;
			sMsg.nLength = 32;
			strcpy(sMsg.arrMsg,"Hello World ! C++");
			SendMsg(&sMsg,sMsg.GetPackLength());
		}
	}
}


void	ClientPlayer::SendMsg( NetMsgHead* pMsg ,int32 nSize)
{
	// 加密处理 
	static char msgBuffer[65536];
	memcpy(&msgBuffer[0],pMsg,nSize);
	Encrypt::XorCode(nSize,m_arrEncryptKey,msgBuffer,nSize);
	pMsg = ( NetMsgHead*)&msgBuffer[0];
	mSocket->ParkMsg(pMsg,nSize);
	mSocket->SendMsg();
}

void					ClientPlayer::SetSocket( NetSocket& rSocket )
{
	mSocket = &rSocket;
}

NetSocket*              ClientPlayer::GetSocket()
{
	return mSocket;
}


ClientPlayerMgr::ClientPlayerMgr()
{
	for (int32 i = 0; i < MAX_CLIENT_COUNT ;++i)
	{
		m_arrClientPlayer[i] = NULL;
		m_arrNetClient[i] = NULL;
	}
}

ClientPlayerMgr::~ClientPlayerMgr()
{

}

void ClientPlayerMgr::Update(int32 nSrvTimes)
{
	for (int32 i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		if(m_arrClientPlayer[i])
		{
			m_arrClientPlayer[i]->Update(nSrvTimes);
		}		
	}
}

void ClientPlayerMgr::AddSocket(NetClient* pNetClient, ClientPlayer* pPlayer)
{
	int32 nIndex = pNetClient->GetSocket().SID() - 1; // socketSID 从1开始 
	m_arrNetClient[nIndex] = pNetClient;
	m_arrClientPlayer[nIndex] = pPlayer;
}


void ClientPlayerMgr::UpdateIO()
{
	for (int32 i = 0; i < MAX_CLIENT_COUNT; ++i)
	{
		if(m_arrNetClient[i])
		{
			m_arrNetClient[i]->Update();
		}		
	}
}


