#pragma once
#include "Executor.h"
#include "Singleton.h"
#include <string>

const size_t LEN_CHAT = 256;
const size_t LEN_NAME = 64;

struct ClientInfo
{
    int ID;
    std::string Name;
    bool Ready;
    SOCKET* TCPSock;
    SOCKADDR_IN* UDPAddr;
};

class AppClient :
    public Singleton<AppClient>,
    public Executor
{
public:
    AppClient();
    ~AppClient();
    std::string Name() override { return "app client"; };

    /*********************************************************
    * @brief    server network object
    ********************************************************/
public:
    class MyClient* m_MyClient = nullptr;

    /*********************************************************
    * @brief    client data
    ********************************************************/
public:
    //client property
    //int m_ID;
    //int m_TCPSocketID;
    bool m_IsConnected;
    char m_Chat[LEN_CHAT];
    char m_Name[LEN_NAME];

    //room data
    std::unordered_map<int, ClientInfo> m_ClientInfos;
    int m_TargetClientNum;

    //for check join
    bool m_JoinSuccess = false;

    //client player
    class Player* m_MainPlayer;

    //client pieces
    //int FinishPieceNum;
    //int TargetPieceNum;
    //std::unordered_map<int, class Piece*> m_Pieces;
    //std::list<class Piece*> m_Bads;
    //std::list<class Piece*> m_Goods;

    //client turn count
    int m_TurnCount;

    //state
    /*int State;
    enum class State_Work
    {
        INIT,
        WORK,
        UNINIT,
    };*/

    /*********************************************************
    * @brief    fixed behaviour
    ********************************************************/
public:
    void Init();
    void Uninit();

    void SendChatMsg();

    void StartInWaitRoomScene();
    void StartInGameScene();
    void LaunchMyClient();

    void StartSelectServer();
    void TryJoinServer();
    void CheckConnect();
    void SetClientID(int id);
    void JoinRoom(int id,int targetNum);//need quit room
    void StartWaitRoom();
    void UpdateClientInfo(int checkMemID, std::string name, bool ready, bool disconnect);
    void RequestDisconnect();
    void Disconnect();//back to select server


    void ResetCameraLookAt(float duration=45);
    void CreatePlayer(PlayerDesc desc);
    void CreatePiece(PieceDesc desc);
    void ChangeToGameScene();
    void ClearPiecesFinishMark();
    void ClearPiecesHandUI();
    void ClearDataInGameScene();

    void StartWaitShowStep(int stepType);
    void StartShowGameOver(int result);

    void StartWaitPiecesFinish();//for pieces show 

    void StartPieceShowEntry(int pieceID, int squareID);
    void StartPieceShowHand(int pieceID);
    void StartPieceShowCheckActpoint(int pieceID, int actPoint);
    void StartPieceInputAct(int pieceID);
    void StartPiecesClearHandAndActpoint();

    void StartPieceShowFinishAct(int pieceID);
    void StartPieceShowMove(int pieceID,int squareID);
    void StartPieceShowCaught(int movePieceID, int caughtPieceID,int prisonRoomSquareID);
    void StartPieceShowEscape(int escapePieceID, int escapeSquareID);

    void ShowPiecesThinkMark();

public:
    class ClientSelectServer* BH_SelectServer;
    class ClientTryConnectServer* BH_TryConnectServer;
    class ClientWaitRoom* BH_WaitStartRoom;

    class ClientWaitPiecesFinish* BH_WaitPiecesFinish;
    class ClientShowStep* BH_ShowStep;
    class ClientShowGameOver* BH_ShowGameOver;
};

