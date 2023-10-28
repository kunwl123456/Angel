#ifndef BASE_NET_DEF_H
#define BASE_NET_DEF_H

#include <fcntl.h>
#include <stdio.h>
#include <wchar.h>
#include <stdarg.h>
#include <errno.h>

#include <stddef.h>
#include <assert.h>
#include <stdarg.h>
#if defined(_WIN32)|| defined(_WIN64)
#include <winSock2.h>
#include <wS2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")

#else
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif

#endif //BASE_NET_DEF_H




























