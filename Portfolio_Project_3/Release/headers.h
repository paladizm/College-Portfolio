/********************************************************
* Author: Zac Paladino
* Class: CPS 472
* Assignment: 1
* Due Date: Feb/16/2010
* File: headers.h
*********************************************************/

#ifndef HEADER_H
#define HEADER_H
#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <queue>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <time.h>

#include "HelperFunctions.h"
using namespace std;

//This is where the peer information is stored.
#pragma pack(push)
#pragma pack(1)
class Peer{
	public:
		DWORD IP;
		unsigned short port;
		bool peer;
		bool leaf;
		bool operator ()(Peer x, Peer y){
			return (memcmp(&x,&y,sizeof(Peer))<0);
		string Host;
		}
};
#pragma pack(pop)
#endif

