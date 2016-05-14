#ifndef CLIENT_PLAYER_H_
#define CLIENT_PLAYER_H_

#include "Client_PCH.h"
#include "../DyNetSocket/NetClient.h"

enum 
{
	MAX_CLIENT_COUNT = 1 ,
};


class ClientPlayer
{
public:
	ClientPlayer(void);
	~ClientPlayer(void);

	void Update(int32 nSrvTimes);

	void SendMsg( NetMsgHead* msg ,int32 nSize);
	void SetSocket( NetSocket& socket );
	NetSocket* GetSocket();
	uint64 GetUid()
	{
		return 0;
	}
	const char*	GetEncryptKey()
	{
		return m_arrEncryptKey;
	}

	void SetEncryptKey(const char arrNewKey[MAX_ENCRYPT_LENTH])
	{
		memcpy(m_arrEncryptKey,arrNewKey,MAX_ENCRYPT_LENTH);
	}

	void SetChating()
	{
		m_bChating = true;
	}

	void SetAccountID(int64 nAccountID)
	{
		m_nAccountID = nAccountID;
	}
	
	int64 AccountID()
	{
		return m_nAccountID;
	}

	void SetCharList(const std::vector<int64>& arrCharID)
	{
		m_nCount = arrCharID.size();
		for (int32 i = 0 ; i < m_nCount; ++i)
		{
			m_arrCharID[i] = arrCharID[i];
		}	
	}

	int64 FirstCharID()
	{
		if(m_nCount)
		{
			return m_arrCharID[0];
		}

		return 0;
	}

protected:

	char m_arrEncryptKey[MAX_ENCRYPT_LENTH];// 通信密钥 
	NetSocket*  mSocket;
	bool m_bChating; // 是否在聊天 
	int64 m_nAccountID;	
	int32 m_nCount;
	int64 m_arrCharID[MAX_ROLE_TYPE_COUNT];
};


class ClientPlayerMgr : public BaseSingle<ClientPlayerMgr>
{
public:

	ClientPlayerMgr();
	~ClientPlayerMgr();

	void Update(int32 nSrvTimes);
	
	void AddSocket(NetClient* pNetClient, ClientPlayer* pPlayer);

	ClientPlayer* GetClientPlayer(NetSocket& rSocket)
	{
		return m_arrClientPlayer[rSocket.SID() - 1];
	}

	void UpdateIO();

	bool CheckConnected();

private:

	NetClient*		m_arrNetClient[MAX_CLIENT_COUNT];
	ClientPlayer*	m_arrClientPlayer[MAX_CLIENT_COUNT];

	std::vector<NetClient*> m_arrWaitingConnect;


};

#endif
