Gnutella Crawler
Programmed By: Zac Paladino
Date: 02/16/2010

Program Instructions:

1. Run the release mode under the comand prompt with the format host[:port][/path] nThreads mPeers with
   n and m specified by the user. (n being the number of threads and m being how many peers you want to
   contact)

2. The crawl will start to run if you give it a correct gnutella host.

3. after the crawl has finished the DNS lookup will happen with the amount of threads that was specified
   under n.

4. after the DNS lookup has finished all of the data will be parsed and put into seperate files.

Output.txt - For Debugging purposes only (the output of the crawl)

peers.txt - shows how many peers and leaves found along with how many were unable to recieve a connection.

DNS.txt - all of the full hostnames (unsorted and uncounted).

time.txt - the amount of time in MS that the crawl took. (only the crawl not the DNS lookups).

HostnameCount.txt - the hostnames and how many times they appeared during the crawl.

CountryCodes.txt - the different Country Codes that were found during the crawl and the amount of times they
		   were recorded.
UserAgents.txt - the different User Agents that were found during the crawl and the amount of times they
		 were recorded.