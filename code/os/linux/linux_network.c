
internal socket_handle
Linux_AcceptSocket(socket_handle Socket, socket_address *Addr, int *AddrLen)
{
    socket_handle Result = accept(Socket, Addr, (socklen_t*)AddrLen);
    return Result;
}

internal b32
Linux_Getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    b32 Result = 0;
    
    if (!getaddrinfo(node, service, hints, res))
    {
        Result = 1;
    }
    
    return Result;
}

internal socket_handle
Linux_MakeSocket(int Domain, int Type, int Protocol)
{
    socket_handle Result = InvalidSocket;
    
    Result = socket(Domain, Type, Protocol);
    
    if (Result == -1)
    {
        Result = InvalidSocket;
    }
    
    return Result;
}


internal socket_handle
Linux_ConnectSocket(socket_handle Socket, const struct sockaddr *Addr, socklen_t AddrLen)
{
    socket_handle Result = 0;
    Result = connect(Socket, Addr, AddrLen);
    if (Result == -1)
    {
        Result = InvalidSocket;
    }
    return Result;
}

internal b32
Linux_Setsockopt(socket_handle Socket, int Level, int OptName, const void *OptVal, socklen_t OptLen)
{
    b32 Result = 0;
    if (!setsockopt(Socket, Level, OptName, (const char *)OptVal, OptLen))
    {
        Result = 1;
    }
    return Result;
}

internal b32
Linux_BindSocket(socket_handle Socket, socket_address *Addr, socklen_t AddrLen)
{
    b32 Result = 0;
    if (!bind(Socket, Addr, AddrLen))
    {
        Result = 1;
    }
    return Result;
}

internal b32
Linux_ListenSocket(socket_handle Socket, int BackLog)
{
    b32 Result = 0;
    if(!listen(Socket, BackLog))
    {
        Result = 1;
    }
    return Result;
}

internal b32
Linux_CloseSocket(socket_handle Socket)
{
    b32 Result = 0;
    
    if (!close(Socket))
    {
        Result = 1;
    }
    
    return Result;
}

internal b32
Linux_SetReadTimeoutOnSocket(socket_handle Socket, r32 Timeout)
{
    b32 Result = 0;
    
    time_t Seconds = (time_t)Timeout;
    u32 MicroSeconds = (u32)((Timeout - Seconds) * 1000000.f);
    
    struct timeval TimeVal;
    TimeVal.tv_sec = Seconds;
    TimeVal.tv_usec = MicroSeconds;
    Result = Linux_Setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&TimeVal, sizeof(TimeVal));
    
    return Result;
}

internal socket_handle 
Linux_OpenClientSocket(char *hostname, char *port)
{
    socket_handle clientfd;
    struct addrinfo hints, *addrinfo_result, *p;
    MemoryZeroStruct(&hints);
    
    // NOTE(fakhri): we want streaming socket, with a numerical port
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
    
    Linux_Getaddrinfo(hostname, port, &hints, &addrinfo_result);
    for (p = addrinfo_result; p; p = p->ai_next)
    {
        // NOTE(fakhri): open client socket
        clientfd = Linux_MakeSocket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (clientfd < 0) continue;
        // NOTE(fakhri): connect to the socket
        if (Linux_ConnectSocket(clientfd, p->ai_addr, (socklen_t)p->ai_addrlen) != -1) break;
        // NOTE(fakhri): close the client socket to avoid memory leak
        Linux_CloseSocket(clientfd);
    }
    freeaddrinfo(addrinfo_result);
    if (!p) return InvalidSocket;
    return clientfd;
}


internal socket_handle 
Linux_OpenListenSocket(char *Port)
{
    socket_handle Result = InvalidSocket;
    addrinfo Hints;
    MemoryZeroStruct(&Hints);
    
    Hints.ai_family = AF_INET;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG | AI_PASSIVE;
    
    addrinfo *AddrInfoResult;
    if (Linux_Getaddrinfo(0, Port, &Hints, &AddrInfoResult))
    {
        addrinfo *Ptr;
        for (Ptr = AddrInfoResult; Ptr; Ptr = Ptr->ai_next)
        {
            
            Result = Linux_MakeSocket(Ptr->ai_family, Ptr->ai_socktype, Ptr->ai_protocol);
            if (Result != InvalidSocket)
            {
                i32 OptionValue = 1;
                Linux_Setsockopt(Result, SOL_SOCKET, SO_REUSEADDR, &OptionValue, sizeof(OptionValue));
                if (Linux_BindSocket(Result, Ptr->ai_addr, (socklen_t)Ptr->ai_addrlen) && 
                    Linux_ListenSocket(Result, LISTENQ))
                {
                    break;
                }
                
                Linux_CloseSocket(Result);
                Result = InvalidSocket;
            }
        }
        freeaddrinfo(AddrInfoResult);
    }
    
    return Result;
}

//~ NOTE(fakhri): io operations


internal network_error
Linux_GetNetworkError()
{
    network_error Result;
    switch(errno)
    {
        case EAGAIN:       Result = network_error_WouldBlock; break;
        case EINTR:        Result = network_error_Interrupted; break;
        case ENOTCONN:     Result = network_error_SocketNotConnnected; break;
        case ENOTSOCK:     Result = network_error_NotSocket; break;
        case EPIPE:        Result = network_error_PeerClosed; break;
        default:           Result = network_error_Unknown; break;
    }
    return Result;
}


internal network_result
Linux_NetworkSendAllBytes(socket_handle Socket, void *Buffer, u32 BytesToSend)
{
    network_result Result;
    Result.Error = network_error_None;
    Result.ProcessedBytes = 0;
    Result.Data = 0;
    
    u32 BytesLeft = BytesToSend;
    i32 SentBytes;
    char *BufferPosition = (char *)Buffer;
    while (BytesLeft)
    {
        SentBytes = send(Socket, BufferPosition, BytesLeft, MSG_NOSIGNAL);
        
        if (SentBytes == -1 )
        {
            Result.Error = Linux_GetNetworkError();
            break;
        }
        
        BytesLeft -= SentBytes;
        BufferPosition += SentBytes;
    }
    
    if (!Result.Error)
    {
        Result.ProcessedBytes = (BytesToSend - BytesLeft);
        Result.Data = Buffer;
    }
    
    return Result;
}

internal network_result
Linux_NetworkReceive(socket_handle Socket, void *Buffer, i32 BufferSize)
{
    network_result Result;
    Result.Error = network_error_None;
    
    i32 BytesRead = recv(Socket, (char *)Buffer, BufferSize, 0);
    if (BytesRead == 0)
    {
        Result.Error = network_error_PeerClosed;
    }
    else if (BytesRead == -1 )
    {
        Result.Error = Linux_GetNetworkError();
    }
    else
    {
        Result.ProcessedBytes = BytesRead;
        Result.Data           = Buffer;
    }
    return Result;
}


internal network_result
Linux_NetworkReceiveNoWait(socket_handle Socket, void *Buffer, i32 BufferSize)
{
    network_result Result;
    Result.Error = network_error_None;
    
    i32 BytesRead = recv(Socket, (char *)Buffer, BufferSize, MSG_DONTWAIT);
    if (BytesRead == 0)
    {
        Result.Error = network_error_PeerClosed;
    }
    else if (BytesRead == -1 )
    {
        Result.Error = Linux_GetNetworkError();
    }
    else
    {
        Result.ProcessedBytes = BytesRead;
        Result.Data           = Buffer;
    }
    return Result;
}

