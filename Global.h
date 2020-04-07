/*
 * Global.h
 *
 *  Created on: Apr 6, 2020
 *      Author: xd
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_
#define PORT 5000
#define BUF_LEN 1024
#define ROOT_PATH "./public/"
#define CONNECTION_OPTION "Connection: keep-alive\r\n"\
															"Keep-Alive: timeout=5, max=1000\r\n"
#define KEEP_ALIVE_TIMEOUT 5
								  //sec usec
#define CGI_TIMEOUT 1,0


#define EC_CON_TIMEOUT 0
#define EC_CON_CLOSE 1

#define DEBUG
#ifdef DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif



#endif /* GLOBAL_H_ */
