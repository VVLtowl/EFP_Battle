#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "SerializedTool.h"

#include <iostream>
using namespace std;


char* EncodeCommonStruct(const CommonStruct& commonStruct, int& outLen)
{
    //分配内存
    outLen = sizeof(long) + commonStruct.len;
    char* strBuff = (char*)malloc(outLen);
    if (nullptr == strBuff)
    {
        return 0;
    }

    //填充内容
    *(long*)strBuff = commonStruct.len;
    if (commonStruct.len > 0)
    {
        memcpy(strBuff + sizeof(long), commonStruct.buff, commonStruct.len);
    }

    return strBuff;
}

bool DecodeCommonStruct(const char* strBuff, long maxLen, CommonStruct& outCommonStruct)
{
    long strlen;
    if (nullptr == strBuff)
    {
        return false;
    }

    //获取到当前长度
    strlen = *(const long*)strBuff;
    cout << "strlen: " << strlen << endl;
    //校验BUFF内容合法性
    if (strlen + sizeof(long) > maxLen)
    {
        return false;
    }

    outCommonStruct.len = strlen;
    outCommonStruct.buff = (void*)malloc(strlen);
    memcpy(outCommonStruct.buff, strBuff + sizeof(long), strlen);
    return true;
}

char* EncodeMsgContent(const MsgContent& msg, char* outBuf ,int msgLen)
{
    //malloc
    int totalLen = sizeof(long) + sizeof(long) + sizeof(int) + msg.DataLen;
    char* strBuff = (char*)malloc(msgLen);
    if (nullptr == strBuff)
    {
        return 0;
    }

    //assign data to buffer
    *(long*)(strBuff) = totalLen;
    *(int*)(strBuff + sizeof(long)) = msg.BHID;
    *(long*)(strBuff + sizeof(long) + sizeof(int)) = msg.DataLen;
    if (msg.DataLen > 0)
    {
        memcpy(strBuff + sizeof(long) + sizeof(int) + sizeof(long),
            msg.Data, msg.DataLen);
    }

    if (outBuf != nullptr)
    {
        memcpy(outBuf, strBuff, msgLen);
    }

    return strBuff;
}

bool DecodeMsgContent(const char* strBuff, MsgContent& outMsg)
{
    long dataLen;
    int bhid;
    long totallen;
    if (nullptr == strBuff)
    {
        return false;
    }

    //get data length
    totallen = *(long*)(strBuff);
    bhid = *(int*)(strBuff + sizeof(long));
    dataLen = *(long*)(strBuff + sizeof(long) + sizeof(int));
    //check valid
    //if (dataLen + sizeof(long) + sizeof(int) > 1024)
    //{
    //    return false;
    //}

    outMsg.BHID = bhid;
    outMsg.DataLen = dataLen;
    outMsg.Data = (void*)malloc(dataLen);
    memcpy(outMsg.Data, strBuff + sizeof(long) + sizeof(int) + sizeof(long), dataLen);
    return true;
}


