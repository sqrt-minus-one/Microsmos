
internal socket_handle
OS_AcceptSocket(socket_handle Socket, socket_address *Addr, int *AddrLen)
{
    socket_handle Result = InvalidSocket;
#if defined(IMPL_AcceptSocket)
    Result = IMPL_AcceptSocket(Socket, Addr, AddrLen);
#else
# error provide an implementation for this
#endif
    return Result;
}

internal socket_handle
OS_MakeSocket(int Domain, int Type, int Protocol)
{
    socket_handle Result = InvalidSocket;
#if defined(IMPL_MakeSocket)
    Result = IMPL_MakeSocket(Domain, Type, Protocol);
#else
# error provide an implementation for this
#endif
    return Result;
}


internal socket_handle
OS_ConnectSocket(socket_handle Socket, const struct sockaddr *Addr, int AddrLen)
{
    socket_handle Result = 0;
#if defined(IMPL_ConnectSocket)
    Result = IMPL_ConnectSocket(Socket, Addr, AddrLen);
#else
# error provide an implementation for this
#endif
    return Result;
}

internal b32
OS_CloseSocket(socket_handle Socket)
{
    b32 Result = 0;
#if defined(IMPL_CloseSocket)
    Result = IMPL_CloseSocket(Socket);
#else
# error provide an implementation for this
#endif
    return Result;
}

internal socket_handle 
OS_OpenClientSocket(char *Hostname, char *Port)
{
    socket_handle Result;
#if defined(IMPL_OpenClientSocket)
    Result = IMPL_OpenClientSocket(Hostname, Port);
#else
# error provide an implementation for this
#endif
    return Result;
}


internal socket_handle 
OS_OpenListenSocket(char *Port)
{
    socket_handle Result = InvalidSocket;
#if defined(IMPL_OpenListenSocket)
    Result = IMPL_OpenListenSocket(Port);
#else
# error provide an implementation for this
#endif
    return Result;
}

//~ NOTE(fakhri): network io

internal network_result
OS_NetworkReceiveAllBytes(socket_handle Socket, void *Buffer, i32 BytesToReceive)
{
    network_result Result;
    Result.Error = network_error_None;
    Result.ProcessedBytes = 0;
    Result.Data = 0;
    
    u32 BytesLeft = BytesToReceive;
    char *BufferPostion = (char *)Buffer;
    while(BytesLeft > 0)
    {
        network_result ReceiveResult = OS_NetworkReceive(Socket, BufferPostion, BytesLeft);
        if (ReceiveResult.Error)
        {
            Result.Error = ReceiveResult.Error;
            break;
        }
        
        BytesLeft -= ReceiveResult.ProcessedBytes;
        BufferPostion += ReceiveResult.ProcessedBytes;
    }
    
    if (!Result.Error)
    {
        Result.ProcessedBytes = (BytesToReceive - BytesLeft);
        Result.Data = Buffer;
    }
    
    return Result;
}

internal network_result
OS_NetworkSendAllBytes(socket_handle Socket, void *Buffer, u32 BytesToSend)
{
    network_result Result;
#if defined(IMPL_NetworkSendAllBytes)
    Result = IMPL_NetworkSendAllBytes(Socket, Buffer, BytesToSend);
#else
    NotImplemented;
#endif
    return Result;
}

internal network_result
OS_NetworkSendStream(socket_handle Socket, string8_list Stream)
{
    network_result Result = ZERO_STRUCT;
    for (string8_node *Node = Stream.First;
         Node;
         Node = Node->Next)
    {
        network_result SendResult = OS_NetworkSendAllBytes(Socket, Node->String.data, (u32)Node->String.size);
        if (SendResult.Error)
        {
            Result.Error = SendResult.Error;
            break;
        }
        Result.ProcessedBytes += SendResult.ProcessedBytes;
    }
    return Result;
}

internal network_result
OS_NetworkReceive(socket_handle Socket, void *Buffer, i32 BufferSize)
{
    network_result Result;
#if defined(IMPL_NetworkReceive)
    Result = IMPL_NetworkReceive(Socket, Buffer, BufferSize);
#else
# error provide an implementation for this
#endif
    return Result;
}

//~ NOTE(fakhri): Buffered sockets

internal buffered_socket *
OS_MakeBufferedSocket(m_arena *Arena, socket_handle Socket)
{
    buffered_socket *Result = (buffered_socket *)M_ArenaPushZero(Arena, sizeof(buffered_socket) + IO_BUFFER_SIZE);
    Result->Socket = Socket;
    Result->Buffer = (b8*)Result + sizeof(buffered_socket);
    Result->Capacity = IO_BUFFER_SIZE;
    return Result;
}

internal network_result
OS_BufferedSocketReceive(buffered_socket *BufferedSocket, void *Buffer, u32 BytesToReceive)
{
    network_result Result;
    Result.Error = network_error_None;
    Result.ProcessedBytes = 0;
    Result.Data = 0;
    
    while(BufferedSocket->AvailableBytes == 0)
    {
        network_result ReceiveResult = OS_NetworkReceive(BufferedSocket->Socket, BufferedSocket->Buffer, BufferedSocket->Capacity);
        
        if (ReceiveResult.Error)
        {
            Result.Error = ReceiveResult.Error;
            break;
        }
        
        BufferedSocket->AvailableBytes = ReceiveResult.ProcessedBytes;
        BufferedSocket->ReadOffest = 0;
        
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
        Result.Data = Buffer;
    }
    return Result;
}


internal network_result
OS_BufferedSocketReceiveLine(m_arena *Arena, buffered_socket *BufferedSocket, u64 LineLengthLimit)
{
    network_result Result = ZERO_STRUCT;
    
    m_temp Scratch = GetScratch(&Arena, 1);
    
    u8 *Buffer = PushArray(Scratch.Arena, u8, LineLengthLimit);
    u64 BufferSize = 0;
    
    
    for(;BufferSize < LineLengthLimit;)
    {
        i8 Character;
        network_result ReceiveResult = OS_BufferedSocketReceive(BufferedSocket, &Character, 1);
        
        if (ReceiveResult.Error) 
        {
            Result.Error = ReceiveResult.Error;
            break;
        }
        
        Buffer[BufferSize++] = Character;
        if (Character == '\n')
        {
            break;
        }
        
    }
    
    if (Buffer[BufferSize - 1] != '\n')
    {
        Result.Error = network_error_Unknown;;
    }
    
    if (!Result.Error)
    {
        string8 StringResult = PushStr8Copy(Arena, Str8(Buffer, BufferSize));
        
        Result.ProcessedBytes = (u32)StringResult.size;
        Result.Data = StringResult.data;
    }
    
    ReleaseScratch(Scratch);
    return Result;
}

internal string8
OS_GetStringFromNetworkResult(network_result NetworkResult)
{
    string8 Result = Str8((u8*)NetworkResult.Data, NetworkResult.ProcessedBytes);
    return Result;
}



internal network_result
OS_BufferedSocketReceiveNoRefill(buffered_socket *BufferedSocket, void *Buffer, u32 BytesToReceive)
{
    network_result Result;
    Result.Error = network_error_None;
    Result.ProcessedBytes = 0;
    Result.Data = 0;
    
    if(BufferedSocket->AvailableBytes == 0)
    {
        Result.Error = network_error_BufferNeedsRefill;
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
        Result.Data = Buffer;
    }
    return Result;
}
