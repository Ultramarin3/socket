
# Socket Programming for Simplified Blockchain System

The blockcain contains 1 Main Server, 2 Clients, and 3 Backend Server. The clients can issue a request for finding their current amount of coins in their account, transfer them to another client and procide a file statement with all the transactions in order. These requests will be sent to a Central Server which in turn interacts with three other backend servers for pulling inforamtion and data processing. The Clients communicate with the Main Server via TCP, and the Main Server communicate with the Backend Server via UDP. 

## CHECK WALLET
For this operation, one Client can fetch the target user's balance in the wallet. The Main Server will interact with 3 Backend Servers to get the information of the target username and retrun the amount of balance

Commandline operation:

`./[clinetname] [username]`

## TXCOINS
For this operation, one Client can transfer coins from one user to another user. The Main Server will interact with 3 Backend Servers process the transation and record it in one of the Backend Servers.

Commandline operation:

`./[clinetname] [sender username] [receiver username] [transfer amout]`

## TXLIST
For this operation, the client willrequest to get the full text version of all the rransactions that have been taking place and save it on a file located on the Main Server.

Commandline operation:

`./[clinetname] TXLIST`
