

a. CHECK WALLET, TXCOINS, TXLIST

b. serverM.c : The main function of serverM, including: receive and handle message from client, send message to client; send message to backend servers, receive and handle message from backend servers; write data to alichain.txt.
   serverA.c : Receive and handle message from main server, send message to main server.
   serverB.c : Receive and handle message from main server, send message to main server.
   serverC.c : Receive and handle message from main server, send message to main server.
   clientA.c : Send message to main server; receive and handle message from main server.
   clientB.c : Send message to main server; receive and handle message from main server.
   func.c : Utility functions that are reused by different servers and clients. Incluing: initialize socket; get records from block.txt; get records from message; sort records; check if user is valid and sufficient; get latest serial number; add records to block.txt

c. Message format of client to serverM: add an operationtype bit at the beginning of input and send it to server M. The operation type is used for serverM to discriminate operations. 
    For example: check wallet
    command: ./clientA Alice
    message send to serverM: "1Alice"
    
    Message format of ServerM to backend server: similar to format above. 
    Operation type 1: check if user is valid and sufficient.
    Operation type 2: check latest serial number.
    Operation type 3: add log to block.txt.
    Operation type 4: get all records from block.txt.
    

d. The project can't handle invalid input (non-numeric transfer number, user name start with number,etc.).

e. The project reference some code from beej's network guide. The TCP server/client and UDP server/client in this project are start from examples in 6 Client-Server Background in beej's guide.                           
   The getBlocktxt(), which reads data from txt into a struct, is reference to an answer in stackoverflow, I'm sorry that I can't find that answer now.
