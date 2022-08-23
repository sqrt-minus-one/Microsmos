/* date = July 22nd 2022 9:46 pm */

#ifndef WIN32_NETWORK_H
#define WIN32_NETWORK_H

#define LISTENQ SOMAXCONN

#define IMPL_AcceptSocket              W32_AcceptSocket
#define IMPL_MakeSocket                W32_MakeSocket
#define IMPL_ConnectSocket             W32_ConnectSocket
#define IMPL_CloseSocket               W32_CloseSocket
#define IMPL_OpenClientSocket          W32_OpenClientSocket
#define IMPL_OpenListenSocket          W32_OpenListenSocket
#define IMPL_NetworkSendBuffer         W32_NetworkSendBuffer
#define IMPL_NetworkReceiveBuffer      W32_NetworkReceiveBuffer
#define IMPL_NetworkReceive            W32_NetworkReceive
#define IMPL_BufferedSocketReceive     W32_BufferedSocketReceive

internal socket_handle W32_AcceptSocket(socket_handle Socket, socket_address *Addr, socklen_t *AddrLen);
internal socket_handle W32_MakeSocket(int Domain, int Type, int Protocol);
internal socket_handle W32_ConnectSocket(socket_handle Socket, const struct sockaddr *Addr, socklen_t AddrLen);
internal b32           W32_CloseSocket(socket_handle Socket);
internal socket_handle W32_OpenClientSocket(char *Hostname, char *Port);
internal socket_handle W32_OpenListenSocket(char *Port);

internal network_result W32_NetworkReceiveBuffer(socket_handle Socket, void *Buffer, i32 BytesToReceive);
internal network_result W32_NetworkSendBuffer(socket_handle Socket, void *Buffer, u32 BytesToSend);
internal network_result W32_NetworkReceive(socket_handle Socket, void *Buffer, i32 BufferSize);
internal network_result W32_BufferedSocketReceive(buffered_socket *BufferedSocket, void *Buffer, u32 BytesToReceive);

#endif //WIN32_NETWORK_H
