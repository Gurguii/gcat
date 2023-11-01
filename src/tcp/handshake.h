/* Gurgui TCP protocol 
 * This file aims to set the behaviour of both the client/server in
 * gurguiprotocol, designed to be used with gurguicat */


/* [PROTOTYPE] Communication will work as following:
 * 1. client -> server : %%BEG%% + server check
 * 2. server -> client : OK
 * 3. client -> server : RDY
 * 4. server -> client : RDY
 * 5. [LOOP] Receive data
 * 5. client -> server : data
*/
#define IFA_ANY "0.0.0.0"
#define IFA_LOOPBACK "127.0.0.1"
