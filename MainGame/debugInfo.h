#pragma once
#include <string>
#include <unordered_map>
#include <functional>

const int BDEBUG_INFO_MAX = 200;


/*********************************************************
* @brief	test block id
********************************************************/
enum TestBlockID
{
	TESTBLOCKID_MAINCAMERA,
	TESTBLOCKID_DEPTHUI,

	TESTBLOCKID_NETWORK_CHAT,

	TESTBLOCKID_CLIENT_WORK,
	TESTBLOCKID_CLIENT_SELECTSERVER,
	TESTBLOCKID_CLIENT_WAITROOM,

	TESTBLOCKID_SERVER_WORK,
	TESTBLOCKID_SERVER_INPUTGAMEROOM,
	TESTBLOCKID_SERVER_WAITCLIENTINROOM,

	TESTBLOCKID_JUDGEMENT_INPUTPLAYMODE,
};

class DebugInfo
{
private:
	static std::string m_Info[BDEBUG_INFO_MAX];
	static bool m_NeedClear;
	
public:
	static std::unordered_map<int, std::function<void()>> TestBlocks;
	static void CloseBlock(int id);
	static void Update();
	static const int Print(std::string info);
	static void PrintRow(int rowID, std::string info);

	//network chat
	static std::string m_NetMsg[BDEBUG_INFO_MAX];
	static void DrawNetMsg();
	static const int PrintNetMsg(std::string info);
};

