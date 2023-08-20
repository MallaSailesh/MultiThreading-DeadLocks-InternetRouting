**Q3**

* Question)How would you handle server failures?
* Ans) By maintaining a backup server .  once the main server got recovered we will switch to it 

**Implementation details**

* client.c :- 
    1) tokenising the input entered in the client side . if its not the type of one of the commands that was asked then  i am prinitng invalid input . If its of the that type i am first creating a socket then initailising the port number and the address then sending data to socket . Then connection is  established if not error is printed . If it is "pt" then client also recieve information regarding the routing table of node 0
    2) Then close the connection 
* server.c :- 
    1) We will create a socket for server and then bind the socket to the address and port number . Now listen on socket (at max i  allowed 50 to access the server at a time). so listen(serverSocket,50) is used . 
    2) We will find the min distance from node 0 to all other nodes  using djikstra , making a small change in dijkstra code gives the parent array . Parent array is the array where it store the parent of node which comes just before the node along the shortest path from 0 to that node .
    3) now in a infinte while loop the server listens to the client . extract the first connection in the queue .message is now recieved at newSocket . Here new socket is used for data transfer and serverSocket for listening 
    4) The recieved message is space and tab tokenised . If tokens[0] = "send" then func1 if tokens[0] = 'pt' then func2 .
    5) In func1 we check the path from node 0 to the destination node mentioned in the command sent by the client (eg :- send 4 Ping! here 4 is destination node) so using parent array i am finding the best route to the destination node from the node 0 . Now when message recieved at a node we will print the source , message recieved , destination , forwaded destination  , destination recieved at each node so on until destination is reached . 
    6) In func2 i am checking the neighbours of node 0 and sending the node value of neigbours , shortest distance to that node(delay) to the client .By  following  the shortest path from node 0 to that destination  ,to which node the data to be sent from node 0 so that it reaches destination in shortest distance  . All these three values are sent for all the neighbours of node 0 . This is recieved by the client who typed pt . This table is called routing table 

