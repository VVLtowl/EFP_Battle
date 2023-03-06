#pragma once
#include "RPS.h"
#include "SerializedTool.h"

class NetworkSendFunc
{
public:
    class MyClient* GetMyClient();
    class MyServer* GetMyServer();
public:
    /*********************************************************
    * @brief    recv func
    ********************************************************/
    void Server_CheckRecv(const MsgContent& msgContent);
    void Client_CheckRecv(const MsgContent& msgContent);

    /*********************************************************
    * @brief    send msg func
    ********************************************************/
    void Client_SendTestMsgToBroadcast(const char* sentence);
    void Server_BroadcastTestMsgToShow(const char* sentence);

    void Client_RequestDisconnect();
    void Server_CommandDisconnect();


    void Client_RequestJoinServer(class AppClient* appClient);
    void Client_TryConnectServer();
    void Server_CommandClientSetID(int setClientID);
    void Server_AgreeJoin(int setClientID,int targetClientNum);
    void Server_SendJoinedClientInfosToClient(int clientID, class AppServer* appServer);
    void Server_DisagreeJoin(int setClientID);
    void Server_CommandUpdateJoinedNum(struct ClientMember* member);
    void Client_RequestSetReady(int clientID, bool ready);


    void Server_CommandLoadGameScene();

    void Judgement_CommandCreatePlayer();
    void Judgement_CommandSetPieceNum();
    void Judgement_CommandCreatePiece(class Piece* piece);
    void Judgement_CommandPieceShowEntry(class Piece* piece);
    void Judgement_CommandShowStep(int showStepType);
    void Judgement_CommandInputRPS();
    void Judgement_CommandShowPieceHand(class Piece* piece);
    void Judgement_CommandPieceShowCheckActpoint(class Piece* piece);
    void Judgement_CommandPieceInputAct(class Piece* piece);

    void Client_NotifyCountPlayerFinished();

    void Piece_RequestSetHand(int pieceID, HandType handType);
    void Judgement_CommandSetHand(int pieceID, HandType handType);

    void Piece_RequestMoveAct(int pieceID, int squareID);
    void Piece_RequestFinishAct(int pieceID);
    void Judgement_CommandPieceFinishAct(int pieceID);
    void Judgement_CommandPieceMove(int pieceID, int squareID);
    void Judgement_CommandPieceCaught(int movePieceID, int caughtPieceID, int prisonRoomSquareID);
    void Judgement_CommandPieceEscape(int pieceID, int goalSquareID);
    void Judgement_CommandPieceContinueMove(int clientID, int pieceID);

    void Judgement_CommandPiecesClearHandAndActpoint();//give up
    void Judgement_CommandPieceClearActpoint(class Piece* piece);
    void Client_RequestUpdatePieceActpoint(class Piece* piece);
    void Judgement_CommandPieceClearHand(class Piece* piece);
    void Client_RequestUpdatePieceHand(class Piece* piece);


    void Judgement_CommandShowGameOver(int result);
};

NetworkSendFunc& GetNetSendFunc();
