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

    /*********************************************************
    * @brief    client data
    ********************************************************/
public:
    //client property
    int ID;
    bool IsConnected;
    int TCPSocketID;
    char Name[64];
    char Chat[128];

    //room data
    int JoinedClientNum;
    int TargetClientNum;
    int ServerPort;
    char ServerIP[16];
    SOCKADDR_IN ServerAddr;
    SOCKET TCPSocket;//for tcp

    //for check join
    bool JoinSuccess = false;

    //client player
    class Player* MainPlayer;

    //client pieces
    //int FinishPieceNum;
    //int TargetPieceNum;
    std::unordered_map<int, class Piece*> Pieces;
    std::list<class Piece*> Bads;
    std::list<class Piece*> Goods;

    //client turn count
    int TurnCount;

    //state
    int State;
    enum class State_Work
    {
        INIT,
        WORK,
        UNINIT,
    };
    enum class State_SelectServer
    {
        SELECT_SERVER,
        WAIT_SERVER_CHECK_JOINABLE,
        FINISH_JOIN,

        NONE,
    };

    /*********************************************************
    * @brief    fixed behaviour
    ********************************************************/
public:
    void SendChatMsg();

    void JoinRoomAndSetID(int id);
    void UpdateNum(int joinedNum, int targetNum);
    void RequestDisconnect();
    void Disconnect();

    void InitGameSceneObjects();
    void CreatePlayer(PlayerDesc desc);
    void CreatePiece(PieceDesc desc);
    void ChangeToGameScene();
    void ClearPiecesFinishMark();

    void StartWaitShowStep(int stepType);

    void StartWaitPiecesFinish();

    void StartPieceShowEntry(int pieceID, int squareID);
    void StartPieceShowHand(int pieceID);
    void StartPieceShowCheckActpoint(int pieceID);
    void StartPieceInputAct(int pieceID);
    void StartPiecesClearHandAndActpoint();

    void StartPieceShowFinishAct(int pieceID);
    void StartPieceShowMove(int pieceID,int squareID);

    void ShowPiecesThinkMark();

public:
    class ClientInit* BH_Init;
    class ClientUninit* BH_Uninit;
    class ClientSelectServer* BH_SelectServer;
    class ClientWaitPiecesFinish* BH_WaitPiecesFinish;
    class ClientShowStep* BH_ShowStep;
};

