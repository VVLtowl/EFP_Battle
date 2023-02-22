#pragma once
#include "Executor.h"
#include "Singleton.h"
#include <string>

class Client :
    public Singleton<Client>,
    public Executor
{
public:
    Client();
    ~Client();
    std::string Name() override { return "client"; };

    /*********************************************************
    * @brief    client data
    ********************************************************/
public:
    //client property
    int m_ID;
    bool m_IsConnected;
    int m_TCPSocketID;
    char m_Name[64];
    char m_Chat[128];

    //room data
    std::unordered_map<int, class ClientMember*> m_ClientInfos;
    int m_TargetClientNum;
    int m_ServerPort;
    char m_ServerIP[16];
    SOCKADDR_IN m_ServerAddr;
    SOCKET m_TCPSocket;//for tcp

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
    void SendChatMsg();

    void StartInWaitRoomScene();
    void StartInGameScene();

    void StartSelectServer();
    void TryJoinServer();
    void CheckConnect();
    void JoinRoom(int id,int targetNum);
    void StartWaitRoom();
    void UpdateClientInfo(int checkMemID, std::string name, bool ready);
    void RequestDisconnect();
    void Disconnect();


    void ResetCameraLookAt(float duration=45);
    void CreatePlayer(PlayerDesc desc);
    void CreatePiece(PieceDesc desc);
    void ChangeToGameScene();
    void ClearPiecesFinishMark();
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
    class ClientInit* BH_Init;
    class ClientUninit* BH_Uninit;
    class ClientSelectServer* BH_SelectServer;
    class ClientTryConnectServer* BH_TryConnectServer;
    class ClientWaitRoom* BH_WaitStartRoom;

    class ClientWaitPiecesFinish* BH_WaitPiecesFinish;
    class ClientShowStep* BH_ShowStep;
    class ClientShowGameOver* BH_ShowGameOver;
};

