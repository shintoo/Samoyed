#!/usr/bin/env bash

cd Chat
gcc TCPChatClient.c ../../src/NetUtil.c -lpthread -o ../chatc
gcc TCPChatServer.c ../../src/TCPServer.c UserNameList.c ../../src/NetUtil.c -lpthread -o ../chats
echo "Chat: server built -> chats"
echo "Chat: client built -> chatc"
cd ..
