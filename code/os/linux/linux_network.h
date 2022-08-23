/* date = June 30th 2022 1:17 pm */

#ifndef LINUX_NETOWRK_H
#define LINUX_NETOWRK_H

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define LISTENQ SOMAXCONN

//~

#define IMPL_AcceptSocket              Linux_AcceptSocket
#define IMPL_MakeSocket                Linux_MakeSocket
#define IMPL_ConnectSocket             Linux_ConnectSocket
#define IMPL_CloseSocket               Linux_CloseSocket
#define IMPL_OpenClientSocket          Linux_OpenClientSocket
#define IMPL_OpenListenSocket          Linux_OpenListenSocket
#define IMPL_NetworkSendAllBytes       Linux_NetworkSendAllBytes
#define IMPL_NetworkReceive            Linux_NetworkReceive

internal socket_handle Linux_AcceptSocket(socket_handle Socket, socket_address *Addr, int *AddrLen);
internal socket_handle Linux_MakeSocket(int Domain, int Type, int Protocol);
internal socket_handle Linux_ConnectSocket(socket_handle Socket, const struct sockaddr *Addr, socklen_t AddrLen);
internal b32           Linux_CloseSocket(socket_handle Socket);
internal socket_handle Linux_OpenClientSocket(char *Hostname, char *Port);
internal socket_handle Linux_OpenListenSocket(char *Port);

internal network_result Linux_NetworkSendAllBytes(socket_handle Socket, void *Buffer, u32 BytesToSend);
internal network_result Linux_NetworkReceive(socket_handle Socket, void *Buffer, i32 BufferSize);

#endif //LINUX_NETOWRK_H
