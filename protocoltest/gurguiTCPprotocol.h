/* Gurgui TCP protocol 
 * This file aims to set the behaviour of both the client/server in
 * gurguiprotocol, designed to be used with gurguicat */


/* [PROTOTYPE] Communication will work as following:
* 1. Client sends %%BEG%% string to the server
* 2. Server sends OK to the client
* 3. Client starts sending data
* @note on each block sent, the server will answer RDY, whenever it got the chunk of data saved, else the client won't keep sending
* 4. Client ends up sending %%END%%
* 5. Server sends DONE and closes connection
*/

#define BEGINNING_OF_CONNECTION_MSG "%%BEG%%"
#define END_OF_CONNECTION_MSG "%%END%%"

#define SERVER_OK "OK"
#define SERVER_END "END"
