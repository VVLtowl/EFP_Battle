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
	TESTBLOCKID_EXECUTORSINFO,
	TESTBLOCKID_MAINCAMERA,
	TESTBLOCKID_DEPTHUI,

	TESTBLOCKID_NETWORK_CHAT,

	TESTBLOCKID_CLIENT_WORK,
	TESTBLOCKID_CLIENT_SELECTSERVER,
	TESTBLOCKID_CLIENT_WAITROOM,
	TESTBLOCKID_CLIENT_CHECKGAMEOVERFINISH,

	TESTBLOCKID_SERVER_WORK,
	TESTBLOCKID_SERVER_INPUTGAMEROOM,
	TESTBLOCKID_SERVER_WAITCLIENTINROOM, 
	TESTBLOCKID_SERVER_SETCLIENTCAMPPIECE,

	TESTBLOCKID_JUDGEMENT_INPUTPLAYMODE,
	TESTBLOCKID_JDUGEMENT_PIECEINFO,
};

class DebugInfo
{
	/*********************************************************
	* @brief	debug infomation 
	********************************************************/
private:
	static std::string m_Info[BDEBUG_INFO_MAX];
	static bool m_NeedClear;
public:
	static void Update();
	static const int Print(std::string info);
	static void PrintRow(int rowID, std::string info);
	static int WriteInfoToLog(std::string fileName);//make log file
	
	/*********************************************************
	* @brief	dynamic test panel
	********************************************************/
public:
	static std::unordered_map<int, std::function<void()>> TestBlocks;
	static void CloseBlock(int id);

	/*********************************************************
	* @brief	net work chat panel
	********************************************************/
public:
	static std::string m_NetMsg[BDEBUG_INFO_MAX];
	static void DrawNetMsg();
	static const int PrintNetMsg(std::string info);
	static void ClearNetMsg();
};



