/* date = June 30th 2022 0:56 pm */

#ifndef OS_NETWORK_H
#define OS_NETWORK_H


typedef struct sockaddr socket_address;
typedef struct addrinfo addrinfo;


#define IO_BUFFER_SIZE Kilobytes(8)
typedef struct buffered_socket buffered_socket;
struct buffered_socket
{
    socket_handle Socket;
    u32 AvailableBytes;
    b8 *Buffer;
    u32 Capacity;
    u32 ReadOffest;
};

typedef enum network_error network_error;
enum network_error
{
    network_error_None = 0,
    network_error_SocketNotConnnected = 1,
    network_error_NotSocket = 2,
    network_error_Timeout = 3,
    network_error_PeerClosed = 4,
    network_error_Interrupted = 5,
    network_error_BufferNeedsRefill = 6,
    network_error_WouldBlock = 7,
    network_error_Unknown,
};

typedef struct network_result network_result;
struct network_result
{
    network_error Error;
    void *Data;
    u32 ProcessedBytes;
};

internal socket_handle  OS_AcceptSocket(socket_handle Socket, socket_address *Addr, int *AddrLen);
internal socket_handle  OS_MakeSocket(int Domain, int Type, int Protocol);
internal socket_handle  OS_ConnectSocket(socket_handle Socket, const struct sockaddr *Addr, int AddrLen);
internal b32            OS_CloseSocket(socket_handle Socket);
internal socket_handle  OS_OpenClientSocket(char *Hostname, char *Port);
internal socket_handle  OS_OpenListenSocket(char *Port);

internal network_result OS_NetworkReceiveAllBytes(socket_handle Socket, void *Buffer, i32 BytesToReceive);
internal network_result OS_NetworkSendAllBytes(socket_handle Socket, void *Buffer, u32 BytesToSend);
internal network_result OS_NetworkSendStream(socket_handle Socket, string8_list Stream);
internal network_result OS_NetworkReceive(socket_handle Socket, void *Buffer, i32 BufferSize);


internal buffered_socket *OS_MakeBufferedSocket(m_arena *Arena, socket_handle Socket);
internal network_result   OS_BufferedSocketReceive(buffered_socket *BufferedSocket, void *Buffer, u32 BytesToReceive);
internal network_result   OS_BufferedSocketReceiveLine(m_arena *Arena, buffered_socket *BufferedSocket, u64 LineLengthLimit);

internal string8 OS_GetStringFromNetworkResult(network_result NetworkResult);

#endif //OS_NETWORK_H
