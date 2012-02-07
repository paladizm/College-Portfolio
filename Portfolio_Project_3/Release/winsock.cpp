/********************************************************
* Author: Zac Paladino
* Class: CPS 472
* Assignment: 1
* Due Date: Feb/16/2010
* File: winsock.cpp
*********************************************************/

#include "headers.h"

//This function deals with the initial HTTP request to start the crawl from the givin
//Gnutella repository.
void winsock_test (int port, string host, string addpath, queue<Peer>* Q, set<Peer,Peer>* Set)
{
	ofstream out;	
	out.open("Output.txt", fstream::app);
	// open a TCP socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
	{
		printf ("socket() generated error %d\n", WSAGetLastError ());
		out << "socket() generated error" << WSAGetLastError () << endl;
		closesocket (sock);	
		return;
	}

	// structure for connecting to server
	struct sockaddr_in server;

	// structure used in DNS lookups
	struct hostent *remote; 

	// first assume that the string is an IP address
	DWORD IP = inet_addr (host.c_str());
	if (IP == INADDR_NONE)
	{
		// if not a valid IP, then do a DNS lookup
		if ((remote = gethostbyname(host.c_str())) == NULL)
		{
			printf ("Invalid string: neither FQDN, nor IP address\n");
			out << "Invalid string: neither FQDN, nor IP address" << endl;
			return;
		}
		else // take the first IP address and copy into sin_addr
			memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
	}
	else
	{
		// if a valid IP, directly drop its binary version into sin_addr
		server.sin_addr.S_un.S_addr = IP;
	}

	// setup the port # and protocol type
	server.sin_family = AF_INET;
	server.sin_port = htons (port);		// host-to-network flips the byte order

	// connect to the server on port 80
	if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf ("Connection error: %d\n", WSAGetLastError ());
		out << "Connection error: " << WSAGetLastError () << endl;
		return;
	}

	printf ("Successfully connected to %s (%s) on port %d\n", host.c_str(), inet_ntoa (server.sin_addr), htons(server.sin_port));
	out << "Successfully connected to " << host.c_str() << " (" << inet_ntoa (server.sin_addr) <<
			") " << "on port " << htons(server.sin_port) << endl;
	//-------------------------- Ready to send/receive -----------------

	// call function send(): send HTTP requests here
	// call function recv(): receive response from the server: 
	int const BUF_SIZE = 8;
	char buf [BUF_SIZE]; //e.g.,BUF_SIZE = 8, prepare to receive data
	string recv_string =""; //used to save all received data
	int bytes = 0;
	int pathlen = static_cast<int>(addpath.length());
	if(addpath[pathlen-1] == '/'){
		addpath.resize(pathlen-1);
	}
	string sstring = "GET ";
	sstring += addpath;
	sstring += "?client=udayton0.2&hostfile=1 HTTP/1.0\nHost:";
	sstring += host;
	sstring += "\n\n";
	
	int bytesSent= send(sock, sstring.c_str(), sstring.length(), 0); //send request
	if (bytesSent < 0){
		printf("Error sending request\n");
		out << "Error sending request\n";
	}
	else{
		fd_set SSOCK;
		SSOCK.fd_count=1;
		SSOCK.fd_array[0] = sock;
		timeval times;
		times.tv_sec=10;
		times.tv_usec=0;
		
		do{
			if(select(NULL,&SSOCK, NULL, NULL, &times)>0){
				if (( bytes =recv (sock, buf, BUF_SIZE-1, 0))==SOCKET_ERROR ){
					printf ("no data received: %d\n", WSAGetLastError ());
					out << "no data received: " << WSAGetLastError () << endl;
					break;
				}else if ( bytes> 0){
					buf[bytes]=0; //NULL terminate buffer
					recv_string += buf; //append to the string
				}
			}
		}while ( bytes > 0);
		printf("%s \n",recv_string.c_str());
		out << recv_string.c_str() << endl;
	}

	//Parse out the seeds.
	vector<string> tokens;
	tokens = MakeTokens(recv_string,"\n");
	vector<string> temp;
	string temp2;
	bool go = false;
	for(int i=0; i < static_cast<int>(tokens.size()); i++){
		temp2 = tokens[i].c_str();
		int t = temp2.size(); 
		if(t == 1){
			go=true;
		}
		else if(go){
			temp.push_back(tokens[i]);
		}
	}
	//error checking
	vector<string> htmlcheck;
	htmlcheck = MakeTokens(tokens[0], " ");
	
	//Throw Seeds into Queue
	if(htmlcheck[2] == "OK\r"){
		vector<string> temp3;	
		for(int x=0; x<static_cast<int>(temp.size());x++){
			temp3 = MakeTokens(temp[x],":");
			Peer z = {inet_addr(temp3[0].c_str()),StringToInt(temp3[1])};
			Q->push(z);
			Set->insert(z);
			temp3.clear();
		}
	}

	//-------------------------- cleanup --------------------------------s

	// close the socket to this server; open again for the next one
	closesocket (sock);

	// call cleanup when done with everything and ready to exit program	
	out.close();
}

//This function is what the threads call to crawl through the Gnutella peers.
vector<vector<Peer>> GNU_winsock_test (Peer p, string& Uagent)
{
	vector<vector<Peer>> peersnleaves;		
	//Initialize WinSock; once per program run
	// open a TCP socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
	{
		//printf ("socket() generated error %d\n", WSAGetLastError ());
		closesocket (sock);	
		return peersnleaves;
	}

	// structure for connecting to server
	struct sockaddr_in server;
	// first assume that the string is an IP address
	DWORD IP = p.IP;
	server.sin_addr.S_un.S_addr = IP;
	// setup the port # and protocol type
	server.sin_family = AF_INET;
	server.sin_port = htons (p.port);		// host-to-network flips the byte order

	// connect to the server on port 80

	if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		//printf ("Connection error: %d\n", WSAGetLastError ());
		closesocket (sock);		
		return peersnleaves;
	}
	//printf ("Successfully connected to %d (%s) on port %d\n", p.IP, inet_ntoa (server.sin_addr), htons(server.sin_port));
	
	//-------------------------- Ready to send/receive -----------------

	// call function send(): send HTTP requests here

	// call function recv(): receive response from the server: 
	int const BUF_SIZE = 512;
	char buf [BUF_SIZE]; //e.g.,BUF_SIZE = 8, prepare to receive data
	string recv_string =""; //used to save all received data
	int bytes = 0;

	char sstring[] = "GNUTELLA CONNECT/0.6\r\nUser-Agent: UDAYTON_CS_CRAWLER77/1.0\r\nX-Ultrapeer: False\r\nCrawler: 0.1\r\n\r\n";
	
	
	int bytesSent= send(sock, sstring, sizeof(sstring), 0); //send request	
	if (bytesSent < 0){
		//printf("Error sending request\n");
		
	}
	else{	
		FD_SET Reader;
		FD_ZERO(&Reader);
		FD_SET( sock, &Reader);   // add your sock to the set Reader

		// set timeout, used for select()
		struct timeval timeout;
		timeout.tv_sec = 10;   // must include <time.h>
		timeout.tv_usec = 0; 

		do{
			if(select(NULL,&Reader, NULL, NULL, &timeout)>0 ){					
				if (( bytes =recv (sock, buf, BUF_SIZE-1, 0))==SOCKET_ERROR ){
					//printf ("no data received: %d\n", WSAGetLastError ());
					bytes=0;
					break;
				}else if ( bytes> 0){
					buf[bytes]=0; //NULL terminate buffer
					recv_string += buf; //append to the string
				}				
			}else
			{
				return peersnleaves;
			}
		}while ( bytes > 0);
		//printf("%s \n",recv_string.c_str());	
	}

	//Parse Gnutella Response
	vector<Peer> peervec, leavevec;
	if(recv_string != ""){
		vector<string> tokens;
		tokens = MakeTokens(recv_string,"\n");		
		vector<vector<string>> temp;
		for(int i=0; i<static_cast<int>(tokens.size()); i++){
			vector<string> temp2;
			temp2 = MakeTokens(tokens[i]," ");
			temp.push_back(temp2);
		}
		string peerst="", leavest="", Userage="";
		for(int z=0; z<static_cast<int>(temp.size()); z++){
			if(static_cast<int>(temp[z].size()) > 0){
				if(!temp[z].empty()){
					if(temp[z][0] == "Peers:"){
						peerst += temp[z][1];
						int pathlen = static_cast<int>(peerst.length());
						if(peerst[pathlen-1] == '\r'){
							peerst.resize(pathlen-1);
						}
					}
					else if(temp[z][0] == "X-Try-Ultrapeers:"){
						if(peerst != ""){
							peerst += ",";
						}
						peerst += temp[z][1];
						int pathlen = static_cast<int>(peerst.length());
						if(peerst[pathlen-1] == '\r'){
							peerst.resize(pathlen-1);
						}
					}
					else if(temp[z][0] == "Leaves:"){
						leavest += temp[z][1];
						int pathlen = static_cast<int>(leavest.length());
						if(leavest[pathlen-1] == '\r'){
							leavest.resize(pathlen-1);
						}
					}
					else if(temp[z][0] == "User-Agent:"){
						for(int i=1; i<static_cast<int>(temp[z].size()); i++){
							Userage += temp[z][i];
							Userage += " ";
						}
						int pathlen = static_cast<int>(Userage.length());
						if(Userage[pathlen-1] == '\r'){
							Userage.resize(pathlen-1);
						}
					}
				}
			}
		}
		Uagent = Userage;
		vector<string> ipaddswp;
		ipaddswp = MakeTokens(peerst, ",");
		vector<string> leavesipwp;
		leavesipwp = MakeTokens(leavest, ",");
		vector<string> temp3;	
		for(int x=0; x<static_cast<int>(ipaddswp.size());x++){
			temp3 = MakeTokens(ipaddswp[x],":");
			if(temp3.size() == 2){
				Peer z = {inet_addr(temp3[0].c_str()),StringToInt(temp3[1])};
				peervec.push_back(z);
			}
			temp3.clear();
		}
		for(int x=0; x<static_cast<int>(leavesipwp.size());x++){
			temp3 = MakeTokens(leavesipwp[x],":");
			if(temp3.size() == 2){
				Peer z = {inet_addr(temp3[0].c_str()),StringToInt(temp3[1])};
				leavevec.push_back(z);
			}
			temp3.clear();
		}			
	}	
	peersnleaves.push_back(peervec);
	peersnleaves.push_back(leavevec);
	//-------------------------- cleanup --------------------------------s

	// close the socket to this server; open again for the next one
	closesocket (sock);
	//return the peers and leaves to get added to the Queue.		
	return peersnleaves;
} 