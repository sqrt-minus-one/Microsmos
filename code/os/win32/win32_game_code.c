

typedef struct w32_game_code w32_game_code;
struct w32_game_code
{
    game_update_and_render_callback *GameUpdateAndRender;
    permanent_load_callbck          *GamePermanentLoad;
    hotload_callback                *GameHotLoad;
    hotunload_callback              *GameHotUnLoad;
    hotload_shader_callback         *GameHotLoadShader;
    
    HMODULE DLL;
    FILETIME LastWriteTime;
};


internal void
W32_GameCodeSetStubs(w32_game_code *GameCode)
{
    GameCode->GameUpdateAndRender = GameUpdateAndRenderStub;
    GameCode->GamePermanentLoad   = GamePermanentLoadStub;
    GameCode->GameHotLoad         = GameHotLoadStub;
    GameCode->GameHotUnLoad       = GameHotUnLoadStub;
    GameCode->GameHotLoadShader   = GameHotLoadShaderStub;
}


internal b32
W32_GameCodeLoad(w32_game_code *GameCode, string8 GameDLLPath, string8 GameTempDLLPath)
{
    b32 result = 1;
    if (!CopyFile(GameDLLPath.cstr, GameTempDLLPath.cstr, FALSE))
    {
        result = 0;
        GetLastError();
        goto end;
    }
    GameCode->DLL = LoadLibraryA(GameTempDLLPath.cstr);
    GameCode->LastWriteTime = W32_GetLastWriteTime(GameDLLPath.cstr);
    
    if(!GameCode->DLL)
    {
        result = 0;
        goto end;
    }
    
    GameCode->GamePermanentLoad    = (permanent_load_callbck *)  GetProcAddress(GameCode->DLL, "GamePermanentLoad");
    GameCode->GameHotUnLoad        = (hotunload_callback *)      GetProcAddress(GameCode->DLL, "GameHotUnLoad");
    GameCode->GameUpdateAndRender  = (game_update_and_render_callback *)GetProcAddress(GameCode->DLL, "GameUpdateAndRender");
    GameCode->GameHotLoad          = (hotload_callback *)        GetProcAddress(GameCode->DLL, "GameHotLoad");
    GameCode->GameHotLoadShader    = (hotload_shader_callback *) GetProcAddress(GameCode->DLL, "GameHotLoadShader");
    
    
    if(!GameCode->GamePermanentLoad || !GameCode->GameHotUnLoad || !GameCode->GameHotLoad || !GameCode->GameUpdateAndRender || !GameCode->GameHotLoadShader)
    {
        W32_GameCodeSetStubs(GameCode);
        result = 0;
        goto end;
    }
    
    end:;
    return result;
}

internal void
W32_GameCodeUnload(w32_game_code *GameCode)
{
    if(GameCode->DLL)
    {
        FreeLibrary(GameCode->DLL);
    }
    GameCode->DLL = 0;
    W32_GameCodeSetStubs(GameCode);
}

internal void
W32_GameCodeUpdate(w32_game_code *GameCode, os_state *OS_State, string8 GameDLLPath, string8 GameTempDLLPath)
{
    FILETIME LastWriteTime = W32_GetLastWriteTime(GameDLLPath.cstr);
    if(CompareFileTime(&LastWriteTime, &GameCode->LastWriteTime))
    {
        GameCode->GameHotUnLoad();
        W32_GameCodeUnload(GameCode);
        W32_GameCodeLoad(GameCode, GameDLLPath, GameTempDLLPath);
        GameCode->GameHotLoad(OS_State);
    }
}
