
internal socket_handle
W32_AcceptSocket(socket_handle Socket, socket_address *Addr, socklen_t *AddrLen)
{
    socket_handle Result = accept(Socket, Addr, AddrLen);
    return Result;
}

internal b32
W32_Getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    b32 Result = 0;
    
    if (!getaddrinfo(node, service, hints, res))
    {
        Result = 1;
    }
    
    return Result;
}

internal socket_handle
W32_MakeSocket(int Domain, int Type, int Protocol)
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
W32_ConnectSocket(socket_handle Socket, const struct sockaddr *Addr, socklen_t AddrLen)
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
W32_Setsockopt(socket_handle Socket, int Level, int OptName, const void *OptVal, socklen_t OptLen)
{
    b32 Result = 0;
    if (!setsockopt(Socket, Level, OptName, (const char *)OptVal, OptLen))
    {
        Result = 1;
    }
    return Result;
}

internal b32
W32_BindSocket(socket_handle Socket, socket_address *Addr, socklen_t AddrLen)
{
    b32 Result = 0;
    if (!bind(Socket, Addr, AddrLen))
    {
        Result = 1;
    }
    return Result;
}

internal b32
W32_ListenSocket(socket_handle Socket, int BackLog)
{
    b32 Result = 0;
    if(!listen(Socket, BackLog))
    {
        Result = 1;
    }
    return Result;
}

internal b32
W32_CloseSocket(socket_handle Socket)
{
    b32 Result = 0;
    
    if (!closesocket(Socket))
    {
        Result = 1;
    }
    
    return Result;
}

internal socket_handle 
W32_OpenClientSocket(char *hostname, char *port)
{
    socket_handle clientfd;
    struct addrinfo hints, *addrinfo_result, *p;
    MemoryZeroStruct(&hints);
    
    // NOTE(fakhri): we want streaming socket, with a numerical port
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
    
    W32_Getaddrinfo(hostname, port, &hints, &addrinfo_result);
    for (p = addrinfo_result; p; p = p->ai_next)
    {
        // NOTE(fakhri): open client socket
        clientfd = W32_MakeSocket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (clientfd < 0) continue;
        // NOTE(fakhri): connect to the socket
        if (W32_ConnectSocket(clientfd, p->ai_addr, (socklen_t)p->ai_addrlen) != -1) break;
        // NOTE(fakhri): close the client socket to avoid memory leak
        W32_CloseSocket(clientfd);
    }
    freeaddrinfo(addrinfo_result);
    if (!p) return InvalidSocket;
    return clientfd;
}


internal socket_handle 
W32_OpenListenSocket(char *Port)
{
    socket_handle Result = InvalidSocket;
    addrinfo Hints;
    MemoryZeroStruct(&Hints);
    
    Hints.ai_family = AF_INET;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG | AI_PASSIVE;
    
    addrinfo *AddrInfoResult;
    if (W32_Getaddrinfo(0, Port, &Hints, &AddrInfoResult))
    {
        addrinfo *Ptr;
        for (Ptr = AddrInfoResult; Ptr; Ptr = Ptr->ai_next)
        {
            
            Result = W32_MakeSocket(Ptr->ai_family, Ptr->ai_socktype, Ptr->ai_protocol);
            if (Result != InvalidSocket)
            {
                i32 OptionValue = 1;
                W32_Setsockopt(Result, SOL_SOCKET, SO_REUSEADDR, &OptionValue, sizeof(OptionValue));
                if (W32_BindSocket(Result, Ptr->ai_addr, (socklen_t)Ptr->ai_addrlen) && 
                    W32_ListenSocket(Result, LISTENQ))
                {
                    break;
                }
                
                W32_CloseSocket(Result);
                Result = InvalidSocket;
            }
        }
        freeaddrinfo(AddrInfoResult);
    }
    
    return Result;
}

internal network_result
W32_NetworkSendBuffer(socket_handle Socket, void *Buffer, u32 BytesToSend)
{
    network_result Result;
    Result.Error = network_error_None;
    Result.ProcessedBytes = 0;
    
    u32 BytesLeft = BytesToSend;
    i32 SentBytes;
    char *BufferPosition = (char *)Buffer;
    while (BytesLeft)
    {
        SentBytes = send(Socket, BufferPosition, BytesLeft, 0);
        
        if (SentBytes == SOCKET_ERROR )
        {
            i32 WSAError = WSAGetLastError();
            switch(WSAError)
            {
                case WSAENOTCONN:  Result.Error = network_error_SocketNotConnnected; break;
                case WSAENOTSOCK:  Result.Error = network_error_NotSocket; break;
                case WSAETIMEDOUT: Result.Error = network_error_Timeout; break;
            }
            if (!Result.Error)
            {
                Result.Error = network_error_Unknown;
            }
            break;
        }
        
        BytesLeft -= SentBytes;
        BufferPosition += SentBytes;
        Result.ProcessedBytes += SentBytes;
    }
    return Result;
}


internal network_result
W32_NetworkReceive(socket_handle Socket, void *Buffer, i32 BufferSize)
{
    network_result Result;
    Result.Error = network_error_None;
    
    i32 BytesRead = recv(Socket, (char *)Buffer, BufferSize, 0);
    if (BytesRead == 0)
    {
        Result.Error = network_error_PeerClosed;
    }
    else if (BytesRead == SOCKET_ERROR )
    {
        i32 WSAError = WSAGetLastError();
        switch(WSAError)
        {
            case WSAENOTCONN:  Result.Error = network_error_SocketNotConnnected; break;
            case WSAENOTSOCK:  Result.Error = network_error_NotSocket; break;
            case WSAETIMEDOUT: Result.Error = network_error_Timeout; break;
        }
        if (!Result.Error)
        {
            Result.Error = network_error_Unknown;
        }
    }
    else
    {
        Result.ProcessedBytes = BytesRead;
    }
    return Result;
}

internal network_result
W32_NetworkReceiveBuffer(socket_handle Socket, void *Buffer, i32 BytesToReceive)
{
    network_result Result;
    Result.Error = network_error_None;
    Result.ProcessedBytes = 0;
    
    u32 BytesLeft = BytesToReceive;
    char *BufferPostion = (char *)Buffer;
    while(BytesLeft > 0)
    {
        network_result ReceiveResult = W32_NetworkReceive(Socket, BufferPostion, BytesLeft);
        if (ReceiveResult.Error)
        {
            Result.Error = ReceiveResult.Error;
            break;
        }
        
        BytesLeft -= ReceiveResult.ProcessedBytes;
        BufferPostion += ReceiveResult.ProcessedBytes;
    }
    Result.ProcessedBytes = (BytesToReceive - BytesLeft);
    return Result;
}


internal network_result
W32_BufferedSocketReceive(buffered_socket *BufferedSocket, void *Buffer, u32 BytesToReceive)
{
    network_result Result;
    Result.Error = network_error_None;
    Result.ProcessedBytes = 0;
    
    while(BufferedSocket->AvailableBytes <= 0)
    {
        network_result ReceiveResult = W32_NetworkReceive(BufferedSocket->Socket, BufferedSocket->Buffer, BufferedSocket->Capacity);
        if (!ReceiveResult.Error)
        {
            BufferedSocket->AvailableBytes = ReceiveResult.ProcessedBytes;
        }
        else
        {
            Result.Error = ReceiveResult.Error;
            break;
        }
    }
    if (!Result.Error)
    {
        u32 BytesToCopy = BufferedSocket->AvailableBytes;
        if (BytesToCopy > BytesToReceive)
        {
            BytesToCopy = BytesToReceive;
        }
        MemoryCopy(Buffer, BufferedSocket->Buffer + BufferedSocket->ReadOffest, BytesToCopy);
        BufferedSocket->ReadOffest += BytesToCopy;
        BufferedSocket->AvailableBytes -= BytesToCopy;
        Result.ProcessedBytes = BytesToCopy;
    }
    return Result;
}
