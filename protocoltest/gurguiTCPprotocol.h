/* Gurgui TCP protocol 
 * This file aims to set the behaviour of both the client/server in
 * gurguiprotocol, designed to be used with gurguicat */


/* [PROTOTYPE] Communication will work as following:
* 1. Client sends %%BEG%% string to the server
* 2. Server sends OK to the client
* 3. Client sends SIZE <size> 
* 4. Server sends OK <size> (client checks if matches) 
* 5. Client starts sending data
* @note server sends OK <read size> in every successful recv()
* and client checks <read size> with it's <write size> before
* sending more data
* 6. Client ends up sending %%END%%
* 7. Server sends DONE and closes connection
*/

#define BEGINNING_OF_CONNECTION_MSG "%%BEG%%"
#define END_OF_CONNECTION_MSG "%%END%%"
#define CONNECTION_DONE "%%DONE%%"
#define SERVER_OK "OK"
#define MAX_PACKET_LENGTH 64000
