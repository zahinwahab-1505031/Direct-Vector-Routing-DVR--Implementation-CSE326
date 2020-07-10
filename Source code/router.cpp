#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

#define INF 99999
int bytes_received;
int sockfd;

class RoutingTableEntry {
public:
	string destination;
	int cost;
	string nextHop;
	RoutingTableEntry(string destination1,int cost1,string nextHop1){
		 destination = destination1;
		 cost = cost1;
		 nextHop = nextHop1;
	}


};

class Neighbour {
public:
	string IPAddress;
	int cost;
	int flag;
	int clock;
	Neighbour(string IPAddress1,int cost1,int flag1){
		IPAddress = IPAddress1;
		cost = cost1;
		flag = flag1;
	}



};

class Router{
public:
	string IPAddress;
    int num_of_routers;
   vector<Neighbour> neighbours;
    set<string> routers;
    vector<RoutingTableEntry> routingTable;
    int clock;
    Router(string IPAddress1){
    	IPAddress = IPAddress1;
    	clock = 0;
    }
    void SetRoutingTable(string filename){
    	 ifstream myfile (filename);
    	  vector <string> tokens; 
    	  string line;
		  if (myfile.is_open())
		  {
		    while ( getline (myfile,line) )
		    {
		      stringstream sstream(line); 
		      
		    string intermediate; 
             while(getline(sstream, intermediate, ' ')) 
		    { if(intermediate!="")
		        tokens.push_back(intermediate); 
		    } 
		    }
		    myfile.close();
		  }
		    for(int i = 0; i < tokens.size(); i++) 
               {
               	if(i%3!=2) {
						if(tokens[i]!=IPAddress) routers.insert(tokens[i]);
					}
               	if(i%3==0) {
               	if(tokens[i]==IPAddress) {

               	
               		 stringstream ss(tokens[i+2]); 
               		 int x;
               		 ss >> x;
               		RoutingTableEntry entry(tokens[i+1],x,tokens[i+1]);
               		Neighbour neighbour(tokens[i+1],x,1);
               		routingTable.push_back(entry);
           			neighbours.push_back(neighbour);
               	}
               }
               if(i%3==1) {
               	if(tokens[i]==IPAddress) {
               		
               		stringstream ss(tokens[i+1]); 
               		 int x;
               		 ss >> x;
               		RoutingTableEntry entry(tokens[i-1],x,tokens[i-1]);
               		routingTable.push_back(entry);
               		Neighbour neighbour(tokens[i-1],x,1);
               		
           			neighbours.push_back(neighbour);
               	}
               }
               }
              set<string>::iterator it = routers.begin();
               int size = routingTable.size();
               		while (it != routers.end())
					{
					string str = *it;
					int f=1;
					//cout << str << endl;
					for(int i=0;i< size;i++) {
						if(str==routingTable[i].destination) {
							
							f=0;
						}
					}
					if(f==1) {
							RoutingTableEntry entry(str,INF,"-1");
               					routingTable.push_back(entry);
					}
					it++;
				
             	    }
             	
            
               num_of_routers = routers.size();
               RoutingTableEntry entry(IPAddress,0,IPAddress);
               	routingTable.push_back(entry);
               SortRoutingTable();
               PrintRoutingTable();
		   }

		      void SortRoutingTable(){
		 
               for(int i=0;i < routingTable.size();i++) {
               	for(int j=0;j<routingTable.size();j++) {
               		if(routingTable[i].destination.compare(routingTable[j].destination)<0) {
               			RoutingTableEntry t = routingTable[i];
               			routingTable[i] = routingTable[j];
               			routingTable[j] = t;
               		}
               	}
						}
						//	cout << "====================================D O N E=========================================================\n";
		   }

		   void PrintRoutingTable(){
		   	cout << "==============================================================================================\n";
		   	  cout << "Printing routing table:\n";
		   	  cout << "Destination         Cost           Next Hop" << endl;
               for(int i=0;i < routingTable.size();i++) {
               	if(routingTable[i].destination.compare(IPAddress)!=0) {
               		if(routingTable[i].cost >= INF) cout << routingTable[i].destination << "         " << "INF" << "          " <<"     -    " << endl;					 
				
					else	cout << routingTable[i].destination << "         " << routingTable[i].cost << "          " <<routingTable[i].nextHop << endl;					 
				
				}
					}
					cout << "====================================D O N E=========================================================\n";
		   }
		   void SendTableToNeighbours(){
		  // 	cout << IPAddress <<  " is sending table to neighbours" << endl;
		  
					for(int j=0;j<neighbours.size();j++)
					{
					 	string table = "tbfn"+IPAddress; //tbfn stands for table for neighbours
		 		    for(int i=0;i < routingTable.size();i++) {
		 		    //	if(routingTable[i].nextHop.compare(neighbours[j].IPAddress)==0)  cout << "bla";
		 		    	if(routingTable[i].nextHop.compare(neighbours[j].IPAddress)==0) table+= "$"+routingTable[i].destination +"#" + to_string(INF) +"#"+"-1";				 
					
					else 
							table+= "$"+routingTable[i].destination +"#" + to_string(routingTable[i].cost) +"#"+routingTable[i].nextHop;					 
					}
		
					string str = neighbours[j].IPAddress;
					struct sockaddr_in router_address;

			        router_address.sin_family = AF_INET;
			        router_address.sin_port = htons(4747);
			        inet_pton(AF_INET,str.c_str(),&router_address.sin_addr);
			       // cout << "trying to send table to " << str << endl;
			        int bytes_sent = sendto(sockfd, table.c_str(), 1024, 0, (struct sockaddr*) &router_address, sizeof(sockaddr_in));
					//if(bytes_sent!=-1) cout << "table sent to " << str << endl;
				
             	    }
		   	}
		   	int GetCostForTheNeighbour(string str){
		   		  for(int i=0;i < neighbours.size();i++) {
		   		  //	cout << neighbours[i].IPAddress << endl;
		   		  	if(str.compare(neighbours[i].IPAddress)==0) {
		   		  		return neighbours[i].cost;
						}
					}

						return INF;
		 	  	
		   }
		   	void SetNewCostForNeighbour(string str,int newCost) {
		   		  for(int i=0;i < neighbours.size();i++) {
		   		  	if(str.compare(neighbours[i].IPAddress)==0)  neighbours[i].cost = newCost;
						}
					
		   	}

		   	 	int GetCostForDestination(string str){
		   		  for(int i=0;i < routingTable.size();i++) {
		   		  	if(str.compare(routingTable[i].destination)==0) return routingTable[i].cost;
						}
						return INF;
		   	}
		   	
		   	string MakeIPAddress(unsigned char * temp)
			{
			    int ipSegment[4];
			    string IPAddress;
			    for(int i = 0; i<4; i++) {
			        ipSegment[i] = temp[i];
			     if(i!=3)   IPAddress+=to_string(ipSegment[i])+".";
			     else if(i==3)   IPAddress+=to_string(ipSegment[i]);
			    }
			     return IPAddress;
			}
		   	void UpdateMyTableAccNewCost(string str,int newCost){
		   		int oldCost = GetCostForTheNeighbour(str);
		   		//cout << "old cost is " << oldCost << endl;
		   		int f=0; 

		   		for(int i=0;i<routingTable.size();i++){
		   			if(routingTable[i].destination.compare(str)==0 && routingTable[i].nextHop.compare(str)==0 ) {
		   				f=1;
		   				routingTable[i].cost = newCost;
		   			}
		   			else if(routingTable[i].destination.compare(str)==0 && routingTable[i].nextHop.compare(str)!=0) {
		   				oldCost = GetCostForDestination(str);
		   			//	cout << "old cost for dest is " << oldCost << endl;
		   				if(newCost<oldCost) {
		   					f=1;
		   					routingTable[i].nextHop = str;
		   					routingTable[i].cost = newCost;

		   				}
		   			}
		   			else if(routingTable[i].destination.compare(str)!=0 && routingTable[i].nextHop.compare(str)==0) {
		   				f=1;
		   				routingTable[i].cost = routingTable[i].cost - oldCost + newCost;
		   			}
		   		}
		   		if(f==1){
		   			cout << "Changed from cost update\n";
		   			PrintRoutingTable();
		   		}
		   	}
		   	void UpdateMyTableForLinkDown(string neighbour){
		   		int f=0;

		   		for(int i=0;i<routingTable.size();i++){
		   			if(routingTable[i].destination.compare(neighbour)==0 && routingTable[i].nextHop.compare(neighbour)==0){
		   				f=1;
		   				routingTable[i].nextHop = "-1";
		   				routingTable[i].cost = INF;
		   			}
		   			else if(routingTable[i].destination.compare(neighbour)!=0 && routingTable[i].nextHop.compare(neighbour)==0){
		   				f=1;
		   				routingTable[i].nextHop = "-1";
		   				routingTable[i].cost = INF;
		   				for(int j=0;j<neighbours.size();j++) {
		   					if(neighbours[j].IPAddress.compare(routingTable[i].destination)==0 && neighbours[j].flag!=-1) {
		   						f=1;
		   						routingTable[i].nextHop = neighbours[j].IPAddress;
		   						routingTable[i].cost = neighbours[j].cost;
		   					}
		   				}
		   			}

		   		}
		   		if(f==1){
		   			cout << "Changed from link failure of " << neighbour << endl;
		   			PrintRoutingTable();
		   		}

		   	}

		   void UpdateMyTableAccMyNeighboursTable(string str){
		    vector<RoutingTableEntry> receivedTable;
	
  	     	vector <string> tokens; 
    	    stringstream sstream(str); 
		      
		    string intermediate; 
             while(getline(sstream, intermediate, '$')) 
		    {
		     if(intermediate!="")
		        tokens.push_back(intermediate); 
		    
		    }
		    string receivedFrom = tokens[0].substr(4);
		    for(int i=0;i<neighbours.size();i++){
		    	if(receivedFrom.compare(neighbours[i].IPAddress)==0) {
		    		neighbours[i].flag = 1;
		    		neighbours[i].clock = clock;
		    	}
		    }
		    for(int i=1;i<tokens.size();i++){
		    	stringstream sstream1(tokens[i]); 
		        vector <string> tokens1; 
				    string intermediate1; 
		             while(getline(sstream1, intermediate1, '#')) 
				    { 
				        tokens1.push_back(intermediate1); 
				    
				    }
					 stringstream ss(tokens1[1]); 
               		 int x;
               		 ss >> x;
               		RoutingTableEntry entry(tokens1[0],x,tokens1[2]);
               		receivedTable.push_back(entry);


		    }
		   
		
		    int f=0;
		    int cost = GetCostForTheNeighbour(receivedFrom);
		   //  cout << "received from" << receivedFrom << "cost is " << cost << endl;
		    for(int i=0;i<routingTable.size();i++){
		    	if(routingTable[i].destination.compare(receivedFrom)==0) {
		    		//	int cost = GetCostForTheNeighbour(receivedFrom);
		    			if(routingTable[i].cost > cost) {
		    			
		    				f=1;
		    				routingTable[i].cost = cost;
		    				routingTable[i].nextHop = receivedFrom;
		    			}

		    	}
		    		/*else if(routingTable[i].nextHop.compare(receivedFrom)==0){
		    				for(int k=0;k<receivedTable.size();k++) {
		    					if(receivedTable[k].destination.compare(routingTable[i].destination)==0) {
		    						int a = receivedTable[k].cost + cost;
		    						int b = routingTable[i].cost;
		    						if(a!=b) {
		    							f=1;
		    							routingTable[i].cost = a;
		    							routingTable[i].nextHop = receivedFrom;
		    						}
		    					}
		    				}
		    		}*/

		    	
		    	/*for(int j=0;j<receivedTable.size();j++) {
		    		if(routingTable[i].destination.compare(receivedTable[j].destination)==0){
		    			

		    			int newCost = cost +  receivedTable[j].cost;
		    			
		    			if(routingTable[i].cost > newCost) {
		    				//cout << routingTable[i].destination << receivedTable[j].destination << "matches yay\n";
		    			    //cout << "old cost changed to " << cost << " new cost  " << newCost << endl;
		    				f=1;
		    				routingTable[i].cost = newCost;
		    				routingTable[i].nextHop = receivedFrom;
		    			}

		    		}
		    		else if(routingTable[i].nextHop.compare(receivedTable[j].destination)==0){
		    			int a = cost + receivedTable[j].cost;


		    		}*/
		    	}
				for(int i = 0; i<routingTable.size(); i++)
			    {
			      
			              int  tempCost = GetCostForTheNeighbour(receivedFrom) + receivedTable[i].cost;
			                if(receivedFrom.compare(routingTable[i].nextHop)==0 && routingTable[i].cost!=tempCost) {
			                		routingTable[i].nextHop = receivedFrom;
			                        routingTable[i].cost = tempCost;
			                        f=1;
			                    
			                }

			                  if((tempCost<routingTable[i].cost && IPAddress.compare(receivedTable[i].nextHop)!=0))
			                {
			                        routingTable[i].nextHop = receivedFrom;
			                        routingTable[i].cost = tempCost;
			                        f=1;
			                    
			                }
			          
			    }
					    
		  
		   if(f==1) {
		   	cout << "------------------------Changed-------------------------\n";
		   	PrintRoutingTable();
		   }
		 //  else cout << "Not Changed\n" ;

		   }
		   string GetIPforNextHop(string dest) {
		   		for(int i = 0; i<routingTable.size(); i++)
				{
					if(!dest.compare(routingTable[i].destination))
					{
						return routingTable[i].nextHop;
						
					}
				}

		   }
		   void forwardMessage(string dest, string length, string msg)
			{
				cout << "In forwarding function\n"<<endl;
				string forwardPckt = "frwd#"+dest+"#"+length+"#"+msg;
				string nextHop = GetIPforNextHop(dest);
			
				struct sockaddr_in router_address;

			    router_address.sin_family = AF_INET;
			    router_address.sin_port = htons(4747);
			    inet_pton(AF_INET,nextHop.c_str(),&router_address.sin_addr);

			    int sent_bytes = sendto(sockfd, forwardPckt.c_str(), 1024, 0, (struct sockaddr*) &router_address, sizeof(sockaddr_in));
				cout<<"trying to forward " << msg.c_str()<<" packet to "<<nextHop.c_str()<<" (printed by "<<IPAddress.c_str()<<")\n";
			
				if(sent_bytes!=-1)cout<<msg.c_str()<<" packet forwarded to "<<nextHop.c_str()<<" (printed by "<<IPAddress.c_str()<<")\n";
			}
			
		   void run(){
		   	struct sockaddr_in router_address;
  		    socklen_t addrlen;
		   	while(true)
			{
				char buffer[1024];
				bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &router_address, &addrlen);
				//printf("[%s:%d]: %s\n", inet_ntoa(router_address.sin_addr), ntohs(router_address.sin_port), buffer);
				if(bytes_received!=-1)
				{
				//	cout << "Driver says: \n" ;
					string str(buffer);
				//	cout << str << endl;

					//cout << "----------------------------------------------------------------------" << endl;
					string command = str.substr(0,4);
					//cout << command << endl;
					if(command.compare("show")==0) PrintRoutingTable();
					else if(command.compare("clk ")==0) {
						clock++;
						SendTableToNeighbours();
						for(int i=0;i<neighbours.size();i++){
							//cout << neighbours[i].IPAddress << endl;
							if((clock-neighbours[i].clock) > 3){
								//cout << "3 clock cycles passed still no news from:   ";
								//cout << neighbours[i].IPAddress << endl;
								neighbours[i].flag=-1;
							//	cout << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! L I N K D O W N !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
								UpdateMyTableForLinkDown(neighbours[i].IPAddress);


							}
						}
					}
					else if(command.compare("tbfn")==0) {
						
						UpdateMyTableAccMyNeighboursTable(str);
						//PrintRoutingTable();
					}

			else if(command.compare("send")==0)
            {
                

				unsigned char *ip1 = new unsigned char[5];
				unsigned char *ip2 = new unsigned char[5];
				
				for(int i = 0; i<4; i++)
				{
					ip1[i] = str[4+i];
					ip2[i] = str[8+i];
				}
                string IPAddress1 = MakeIPAddress(ip1);
                string IPAddress2 = MakeIPAddress(ip2);

				unsigned char *c1 = new unsigned char[3];
				string msgLength = str.substr(12,2);
				for(int i=0;i<2;i++) {
					c1[i] =  str[12+i];
				}
				int length = 0;
				
				int x0,x1;
				x0 = c1[0];
				x1 = c1[1]*256;
				length = x1+x0;
				string message = "";
                for (int i=0; i<length; i++) {
                    message+= buffer[14+i];
                }
				
				if(IPAddress2.compare(IPAddress)==0)
				{
					cout<<message<<" packet reached destination (printed by "<<IPAddress2<<")\n";
				}
				else
					forwardMessage(IPAddress2,msgLength,message);
          	  }
			  else if(command.compare("frwd")==0)
	            {
	                vector <string> tokens; 
		        	stringstream sstream(str); 
				      
				    string intermediate; 
		            while(getline(sstream, intermediate, '#')) 
				    { 
				    	if(intermediate!="")
				        tokens.push_back(intermediate); 
				    
				    }

					//forwarding function
					if(!tokens[1].compare(IPAddress))
					{
						cout<<tokens[3]<<" packet reached destination (printed by "<<tokens[1]<<")\n";
					}
					else
						forwardMessage(tokens[1],tokens[2],tokens[3]);
					
	            }
            		else if(command.compare("cost")==0) {
							unsigned char *c1 = new unsigned char[3];
							for(int i=0;i<2;i++) {
								c1[i] =  str[12+i];
							}
							int a,b;
							a = c1[0];
							b = c1[1]*256;
							int newCost = a+b;
							unsigned char *ip1 = new unsigned char[5];
							unsigned char *ip2 = new unsigned char[5];
							
							for(int i = 0; i<4; i++)
							{
								ip1[i] = str[4+i];
								ip2[i] = str[8+i];
							}
			                string IPAddress1 = MakeIPAddress(ip1);
			                string IPAddress2 = MakeIPAddress(ip2);
			                if(IPAddress1.compare(IPAddress)==0) {
			                	UpdateMyTableAccNewCost(IPAddress2,newCost) ;
			                	SetNewCostForNeighbour(IPAddress2,newCost) ;
			                }
			                else if(IPAddress2.compare(IPAddress)==0) {
			                	UpdateMyTableAccNewCost(IPAddress1,newCost) ;
		                		SetNewCostForNeighbour(IPAddress1,newCost) ;
			                }



						
					}
				}
				//sleep(1);
			}
		}
	


		   
};
int main(int argc, char *argv[]){

	
	
	char buffer[1024];
	

	if(argc != 3){
		printf("%s <ip address>\n", argv[1]);
		exit(1);
	}
	
	Router router(argv[1]);
	cout << "Router's IP Address: " << router.IPAddress << endl;
//	printf("Router's IP address: %s",IPAddress);
	
    int bind_flag;
    struct sockaddr_in client_address;

    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(4747);
    inet_pton(AF_INET, argv[1], &client_address.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));

	if(!bind_flag) cout<<"Connection successful :) "<<endl;
	else {
		cout<<"Connection failed :( "<<endl;
		close(sockfd);
		return 0;
	}
	router.SetRoutingTable(argv[2]);
	cout << "====================Neighbour are ===========================================================\n";
	for(int i=0;i<router.neighbours.size();i++) {
		cout << router.neighbours[i].IPAddress << " " << router.neighbours[i].cost<< endl;
	}
    cout<<"--------------------------------------"<<endl;
    router.run();

	cout << "Exiting ................................\n";
	close(sockfd);
	return 0;

}
