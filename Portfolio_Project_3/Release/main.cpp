/********************************************************
* Author: Zac Paladino
* Class: CPS 472
* Assignment: 1
* Due Date: Feb/16/2010
* File: main.cpp
*********************************************************/
#include "headers.h"

// this class is passed to all threads, acts as shared memory
class Parameters{
public:
	HANDLE mutex;
	HANDLE finished;
	HANDLE eventQuit;
	HANDLE mutex2;
	HANDLE finished2;
	HANDLE eventQuit2;
	queue<Peer> *Q;
	queue<Peer> *DNSQ;
	set<Peer,Peer> *Set;
	vector<vector<string>> *UserAgents;
	vector<string> *hostnames;	
	int activeT, MAX_ULPEER, visitedUlpeers,activeT2, visitedUlpeers2, peerc, leafc, failedconn;	
};

// function inside winsock.cpp
void winsock_test (int port, string host, string addpath, queue<Peer>* Q, set<Peer,Peer>* Set);
vector<vector<Peer>> GNU_winsock_test (Peer p, string& Uagent);

// this function is where the thread starts
UINT thread(LPVOID pParam)
{ 
	Parameters *p = ((Parameters*)pParam); 
	HANDLE arr[] = {p->eventQuit,p->finished};
	string Uagent;
	ofstream out;
	bool go=false;	
	int result;
	while(true){
		result = WaitForMultipleObjects(2, arr, false, INFINITE); // decrease count by one 		
		if ( result - WAIT_OBJECT_0 == 0) // the eventQuit has been signaled 
		{
			WaitForSingleObject(p->mutex, INFINITE);
			printf("Thread Exiting\n");
			ReleaseMutex(p->mutex);
			return 0; 
		}		
		Peer temp;	
		// wait for mutex, then print and sleep inside the critical section			
		WaitForSingleObject(p->mutex, INFINITE);				// lock mutex		
		printf ("Thread %d started\n", GetCurrentThreadId ());		// always print inside critical section to avoid screen garbage
		vector<vector<Peer>> pandls;
		go = false;
		if(!p->Q->empty()){
			p->activeT++;
			temp = p->Q->front();
			p->Q->pop();
			go=true;
		}
		ReleaseMutex(p->mutex);									// release critical section
		if(go){
			pandls=GNU_winsock_test(temp, Uagent);
			if(!pandls.empty() && (p->visitedUlpeers < p->MAX_ULPEER)){
				WaitForSingleObject(p->mutex, INFINITE);
				out.open("Output.txt", fstream::app);
				out << "Thread " << GetCurrentThreadId() << " Crawled Peer and is Adding to Q." << endl;
				out.close();
				printf("Thread %d Crawled Peer and is Adding to Q \n", GetCurrentThreadId ());
				for(int i=0; i<static_cast<int>(pandls[0].size()); i++){
					pandls[0][i].peer = true;
					p->Q->push(pandls[0][i]);
					p->Set->insert(pandls[0][i]);
					p->peerc++;
				}
				if(pandls.size() == 2){
					for(int i=0; i<static_cast<int>(pandls[1].size()); i++){
						pandls[1][i].leaf = true;	
						p->Set->insert(pandls[1][i]);
						p->leafc++;
					}
				}
				p->visitedUlpeers++;
				printf("%d\n", p->visitedUlpeers);
				if(Uagent != ""){
					bool newuser = true;
					for(int i=0; i < static_cast<int>(p->UserAgents->size()); i++){
						if(p->UserAgents[0][i][0] == Uagent){
							string temp = p->UserAgents[0][i][0];
							int tmp = StringToInt(p->UserAgents[0][i][1]);
							tmp++;
							char buf[256];
							p->UserAgents[0][i][1] = itoa(tmp, buf, 10);							
							newuser=false;
							break;
						}
					}
					if(newuser){
						vector<string> tempua;
						tempua.push_back(Uagent);
						tempua.push_back("1");
						p->UserAgents->push_back(tempua);
					}
				}
				p->activeT--;
				for(int i=0; i<static_cast<int>(pandls[0].size()); i++){
					ReleaseSemaphore(p->finished, 1, NULL);	
				}				
				if((p->Q->size()==0 && p->activeT == 0) || p->visitedUlpeers >= p->MAX_ULPEER){
					SetEvent(p->eventQuit);
				}
				ReleaseMutex(p->mutex);
			}
			else if(p->visitedUlpeers < p->MAX_ULPEER){
				WaitForSingleObject(p->mutex, INFINITE);
				p->activeT--;
				p->visitedUlpeers++;
				out.open("Output.txt", fstream::app);
				out << "Thread " << GetCurrentThreadId() << " Crawled Peer and there was a connection/data error." << endl;
				out.close();
				printf("Thread %d Crawled Peer and there was a connection/data error\n", GetCurrentThreadId ());
				printf("%d\n", p->visitedUlpeers);
				p->peerc++;
				p->failedconn++;									
				if((p->Q->size()==0 && p->activeT == 0) || p->visitedUlpeers >= p->MAX_ULPEER){
					SetEvent(p->eventQuit);
				}
				ReleaseMutex(p->mutex);
			}
		}		
	}	
}

//This is the thread function for the DNS lookups.
UINT DNSthread(LPVOID pParam)
{
	Parameters *p = ((Parameters*)pParam);

	HANDLE arr[] = {p->eventQuit2,p->finished2};
	string Uagent;
	bool go=false;
	srand(time(NULL));
	int result;
	while(true){
			result = WaitForMultipleObjects(2, arr, false, INFINITE); // decrease count by one 			
			if ( result - WAIT_OBJECT_0 == 0) // the eventQuit has been signaled 
			{
				WaitForSingleObject(p->mutex, INFINITE);
				printf("Thread Exiting\n");
				ReleaseMutex(p->mutex);
				return 0; 
			}		
			Peer temp;	
			// wait for mutex, then print and sleep inside the critical section			
			WaitForSingleObject(p->mutex2, INFINITE);					// lock mutex
			printf ("Thread %d started\n", GetCurrentThreadId ());	// always print inside critical section to avoid screen garbage			
			go = false;
			if(!p->DNSQ->empty()){
				p->activeT2++;
				temp = p->DNSQ->front();
				p->DNSQ->pop();
				go=true;
			}
			ReleaseMutex(p->mutex2);									// release critical section
			if(go){
				struct sockaddr_in server;
				char hostname[NI_MAXHOST];
				char servInfo[NI_MAXSERV];
				u_short prt = temp.port;
				server.sin_addr.S_un.S_addr = temp.IP;
				server.sin_family = AF_INET;
				server.sin_port = htons(prt);
				int dwRetval;							
				dwRetval = getnameinfo((struct sockaddr *) &server,
							   sizeof (struct sockaddr),
							   hostname,
							   NI_MAXHOST, servInfo, 
							   NI_MAXSERV, NI_NUMERICSERV);
				if(dwRetval == 0){									
					WaitForSingleObject(p->mutex2, INFINITE);
					ofstream out;
					out.open("DNS.txt", fstream::app);
					out << hostname << endl;
					out.close();
					p->hostnames->push_back(hostname);													
					p->activeT2--;
					ReleaseSemaphore(p->finished2, 1, NULL);	
					if(p->DNSQ->size()==0){
						SetEvent(p->eventQuit2);
					}
					ReleaseMutex(p->mutex2);
				}
			}
			int t = rand() % 10000;
			Sleep(10000);
			// signal that this thread has exitted			
	}	
}

//This function parses the initial HTTP Connection.
void ParseUltrapeer(int& port, int& nThreads, int& mUlts, string& host, string& addpath,int argc, char *argv[]){
	string stopar="";
	for(int i=0; i<argc; i++){
		if(i!=0){
			stopar += argv[i];
			stopar += " ";
		}
	}
	cout << stopar << endl;
	vector<string> tokens;
	string temp="";
	string delim = " ";
	tokens = MakeTokens(stopar,delim);
	string stopar2 = "";
	if(!tokens.empty()){
		stopar2 = tokens[0];
		if(tokens.size() >= 2){
			nThreads = StringToInt(tokens[1]);
		}
		if(tokens.size() >= 3){
			mUlts = StringToInt(tokens[2]);
		}
	}
	bool prt=false,apath=false;
	for(int i=0; i < static_cast<int>(stopar2.length()); i++){
		if((stopar2[i] == ':' || stopar2[i] == '/') && !prt && !apath){
			if(stopar2[i] == ':'){
				prt = true;
			}
			else{
				addpath += stopar2[i];
				apath = true;
			}
		}
		else if(prt){
			if(stopar2[i] != '/'){
				temp += stopar2[i];
			}
			else{
				port = StringToInt(temp);
				addpath = stopar2[i];
				apath = true;
				prt = false;
			}
		}
		else if(apath){
			addpath += stopar2[i];
		}
		if(!prt && !apath){
			host += stopar2[i];
		}
	}
}


int main(int argc, char *argv[])
{	
	//These are the country coes that will be used for the DNS parsing.
	string CCodes[245] = {"AF","AL","DZ","AS","AD","AO","AI","AQ","AG","AR","AM","AW","AU","AT","AZ","BS","BH","BD","BB","BY","BE","BZ",
					"BJ","BM","BT","BO","BA","BW","BV","BR","IO","BN","BG","BF","BI","KH","CM","CA","CV","KY","CF","TD","CL",
					"CN","CX","CC","CO","KM","CG","CD","CK","CR","CI","HR","CU","CY","CZ","DK","DJ","DM","DO","TP","EC","EG","SV",
					"GQ","ER","EE","ET","FK","FO","FJ","FI","FR","GF","PF","TF","GA","GM","GE","DE","GH","GI","GR","GL","GD","GP",
					"GU","GT","GN","GW","GY","HT","HM","VA","HN","HK","HU","IS","IN","ID","IR","IQ","IE","IL","IT","JM","JP","JO",
					"KZ","KE","KI","KP","KR","KW","KG","LA","LV","LB","LS","LR","LY","LI","LT","LU","MO","MK","MG","MW","MY","MV",
					"ML","MT","MH","MQ","MR","MU","YT","MX","FM","MD","MC","MN","MS","MA","MZ","MM","NA","NR","NP","NL","AN","NC",
					"NZ","NI","NE","NG","NU","NF","MP","NO","OM","PK","PW","PS","PA","PG","PY","PE","PH","PN","PL","PT","PR","QA",
					"RE","RO","RU","RW","SH","KN","LC","PM","VC","WS","SM","ST","SA","SN","SC","SL","SG","SK","SI","SB","SO","ZA",
					"GS","ES","LK","SD","SR","SJ","SZ","SE","CH","SY","TW","TJ","TZ","TH","TG","TK","TO","TT","TN","TR","TM","TC",
					"TV","UG","UA","AE","GB","US","UM","UY","UZ","VU","VE","VN","VG","VI","WF","EH","YE","YU","ZM","ZW","COM","INFO",
					"BIZ","EDU","NET","GOV"};
	ofstream out;
	out.open("Output.txt");
	out.close();
	out.open("DNS.txt");
	out.close();
	Parameters p;
	int port = 80, nThreads=1, mUlts=1;
	p.Q = new queue<Peer>;
	p.DNSQ = new queue<Peer>;
	p.Set=new set<Peer,Peer>;
	p.UserAgents= new vector<vector<string>>;
	p.hostnames = new vector<string>;
	string host="";
	string addpath="";
	ParseUltrapeer(port,nThreads,mUlts, host, addpath, argc, argv);	
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2,2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("WSAStartup error %d\n", WSAGetLastError ());
		WSACleanup();		
	}
	winsock_test(port,host,addpath,p.Q, p.Set);
	if(!p.Q->empty()){
		p.MAX_ULPEER=mUlts;
		p.activeT=0;
		p.visitedUlpeers=0;
		p.peerc=0;
		p.leafc=0;
		p.failedconn=0;
		printf ("-----------------\n");
		// thread handles are stored here; they can be used to check status of threads, or kill them
		HANDLE *ptrs = new HANDLE [nThreads];
		
		
		// create a mutex for accessing critical sections (including printf)
		p.mutex = CreateMutex (NULL, 0, NULL);
		
		// create a semaphore that counts the number of active threads
		p.finished = CreateSemaphore(NULL, 0, nThreads, NULL);
		p.eventQuit = CreateEvent (NULL, true, false, NULL);

		// get current time
		DWORD t = timeGetTime();

		// structure p is the shared space between the threads
		for(int i=0; i<nThreads; i++){
			printf("-----------created thread-----\n"); 
			ptrs[i] = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)thread, &p, 0, NULL);
			SetThreadPriority(ptrs[i], THREAD_PRIORITY_LOWEST);
		}
		ReleaseSemaphore(p.finished,min(nThreads,static_cast<int>(p.Q->size())),NULL);	
		// make sure this thread hangs here until the other two quit; otherwise, the program will terminate prematurely
		WaitForSingleObject(p.eventQuit, INFINITE);	
		printf ("Terminating Gnutella Crawl, completion time %d ms\n", timeGetTime()-t); 
		out.open("time.txt");
		out << timeGetTime()-t;
		out.close();
		printf ("Cleaning Up Threads...\n");
		
		//pushing the peer set into the DNSQ so the hostname can be found.
		if(!p.Set->empty()){		
			set<Peer,Peer>::iterator i;		
			for(i = p.Set->begin(); i!=p.Set->end(); ++i){
				Peer tr = *i;
				p.DNSQ->push(tr);			
			}
		}
		cout << "Printing Peers to file..." << endl;
		out.open("peers.txt");
		if(!p.Set->empty()){
			out << "Peers: " << p.peerc << endl;
			out << "Leafs: " << p.leafc << endl;
			out << "Failed Connections: " << p.failedconn << endl;
			out << "Set of Peers:" << endl;
			set<Peer,Peer>::iterator i;
			struct sockaddr_in server;
			for(i = p.Set->begin(); i!=p.Set->end(); ++i){
				server.sin_addr.S_un.S_addr = i->IP;
				out << inet_ntoa(server.sin_addr) << ":" << i->port << endl;
			}
		}
		out.close();
		//finished the Gnutella Crawl now doing the DNS lookup.
		cout << "Starting DNS Lookup" << endl;
		if(!p.DNSQ->empty()){
			p.activeT2=0;
			p.visitedUlpeers2=0;
			printf ("-----------------\n");
			// thread handles are stored here; they can be used to check status of threads, or kill them
			HANDLE *ptrs = new HANDLE [nThreads];
			
			
			// create a mutex for accessing critical sections (including printf)
			p.mutex2 = CreateMutex (NULL, 0, NULL);
			
			// create a semaphore that counts the number of active threads
			p.finished2 = CreateSemaphore(NULL, 0, nThreads, NULL);
			p.eventQuit2 = CreateEvent (NULL, true, false, NULL);

			// get current time
			DWORD t = timeGetTime();

			// structure p is the shared space between the threads
			for(int i=0; i<nThreads; i++){
				printf("-----------created thread-----\n"); 
				ptrs[i] = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)DNSthread, &p, 0, NULL);
			}
			ReleaseSemaphore(p.finished2,nThreads,NULL);	
			// make sure this thread hangs here until the other two quit; otherwise, the program will terminate prematurely
			WaitForSingleObject(p.eventQuit2, INFINITE);	
			printf ("Terminating DNS Lookup, completion time %d ms\n", timeGetTime()-t); 		
			printf ("Cleaning Up Threads...\n");
		}
		//Finished the DNS lookup now parsinf the information that was recieved into usable data.
		vector<vector<string>> hostpar;
		vector<vector<string>> CountryCount;
		vector<vector<string>> DomainCount;
		if(!p.hostnames->empty()){
			for(int i=0;i < static_cast<int>(p.hostnames->size());i++){
				vector<string> temp;
				temp = MakeTokens(p.hostnames[0][i],".");
				hostpar.push_back(temp);
			}
			cout << "Parsed Hostnames" << endl;
			for(int i=0; i < static_cast<int>(hostpar.size());i++){
				string domain="";
				int wcount=0;
				for(int z=static_cast<int>(hostpar[i].size())-1; z>=0; z -= 1){					
					for(int t=0; t<static_cast<int>(hostpar[i][z].length()); t++){
						if(!isdigit(hostpar[i][z][t])){
							hostpar[i][z][t] = toupper(hostpar[i][z][t]);
						}
						else{
							hostpar[i][z][t] = hostpar[i][z][t];
						}
					}		
					string tempt = hostpar[i][z];
					bool notdom = false;
					for(int k=0; k<245; k++){						
						if(tempt == CCodes[k]){
							wcount++;
							notdom = true;
							break;
						}
					}
					if(!notdom){												
						wcount++;
						break;
					}
					if(z == static_cast<int>(hostpar[i].size()-1)){
						bool newcc = true;
						for(int x=0; x < static_cast<int>(CountryCount.size()); x++){
							if(CountryCount[x][0] == hostpar[i][z]){
								string temp = CountryCount[x][1];
								int tmp = StringToInt(CountryCount[x][1]);
								tmp++;
								char buf[256];
								CountryCount[x][1] = itoa(tmp, buf, 10);							
								newcc=false;
								break;
							}
						}
						if(newcc){
							vector<string> tempua;
							tempua.push_back(hostpar[i][z]);
							tempua.push_back("1");
							CountryCount.push_back(tempua);
						}
					}				
				}
				for(int u= static_cast<int>(hostpar[i].size())-(wcount); u<static_cast<int>(hostpar[i].size()); u++){
					bool nogo = false;
					for(int t=0; t<static_cast<int>(hostpar[i][u].length()); t++){
						if(isdigit(hostpar[i][u][t])){
							nogo=true;
							break;
						}				
					}	
					if(!nogo){
						domain += hostpar[i][u];
						if(u+1 < static_cast<int>(hostpar[i].size())){
							domain += ".";
						}
					}
				}
				if(domain != ""){
					bool newdomain = true;
					for(int x=0; x < static_cast<int>(DomainCount.size()); x++){
						if(DomainCount[x][0] == domain){
							string temp = DomainCount[x][1];
							int tmp = StringToInt(DomainCount[x][1]);
							tmp++;
							char buf[256];
							DomainCount[x][1] = itoa(tmp, buf, 10);							
							newdomain=false;
							break;
						}
					}
					if(newdomain){
						vector<string> tempua;
						tempua.push_back(domain);
						tempua.push_back("1");
						DomainCount.push_back(tempua);
					}
				}				
			}
		}
		cout << "finished DNS" << endl;	
		//Finished parsing the data.
		//Dump User Agent Names and their count into a file.
		out.open("UserAgents.txt");
		out << "User Agents:" << endl;
		for(int i=0; i < static_cast<int>(p.UserAgents->size()); i++){
			int pathlen = static_cast<int>(p.UserAgents[0][i][0].length());
			if(p.UserAgents[0][i][0][pathlen-1] == '\r'){
				p.UserAgents[0][i][0].resize(pathlen-1);
			}
			out << p.UserAgents[0][i][0] << " " << p.UserAgents[0][i][1] << endl;
		}
		out.close();
		//Dump Host Names and their count into a file.
		out.open("HostnameCount.txt");
		out << "Hostnames:" << endl;
		for(int i=0; i < static_cast<int>(DomainCount.size()); i++){		
			out << DomainCount[i][0] << " " << DomainCount[i][1] << endl;
		}
		out.close();
		//Dump Country Codes and their count into a file.
		out.open("CountryCodes.txt");
		out << "Country Codes:" << endl;
		for(int i=0; i < static_cast<int>(CountryCount.size()); i++){		
			out << CountryCount[i][0] << " " << CountryCount[i][1] << endl;
		}
		out.close();
	}
	WSACleanup();
	return 0; 
}
