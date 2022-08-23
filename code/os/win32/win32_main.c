
#include <windows.h>
#include <windowsx.h>
#include <xinput.h>

// NOTE(fakhri): definitions

#include "third_party/glad/glad.h" 
#include "third_party/wglext.h"

#include "base/base_inc.h"
#include "os/os_inc.h"

#include "game/game_inc.h"

// NOTE(fakhri): global variables
global game_input W32_GameInput;
global w32_timer W32_Timer;
global os_state W32_OS;
global m_arena *W32_PermArena = 0;
global string8 W32_InitalPath = {0};
global string8 W32_ExecutablePath = {0};
global string8 W32_GameDLLPath = {0};
global string8 W32_GameTempDLLPath = {0};
global thread_ctx W32ThreadContext;
global HWND w32_window_handle;

#define W32_GraphicalWindowClassName L"ApplicationWindowClass"
#define W32_GraphicalWindowTitle     L"Microsmos"
#define W32_GraphicalWindowWidth     1280u
#define W32_GraphicalWindowHeight    720u 
#define Application_Name             "Microsmos"
// NOTE(fakhri): implementations
#include "third_party/glad/glad.c"
#include "base/base_inc.c"
#include "os/os_inc.c"
//#include "game/game_inc.c"

internal LRESULT
W32_WindowProc(HWND HWnd, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    os_event *Event = 0;
    
    b32 is_release = 0;
    Axis2 scroll_axis = Axis2_Y;
    switch(Message)
    {
        default:
        {
            Result = DefWindowProcW(HWnd, Message, WParam, LParam);
        } break;
        
        case WM_CLOSE:
        {
            W32_OS.Quit = true;
        }break;
        
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            is_release = true;
        } fallthrough;
        case WM_MBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        {
            
            os_event_kind Kind = is_release ? OS_EventKind_Release : OS_EventKind_Press;
            os_key Key = OS_Key_MouseLeft;
            switch(Message)
            {
                case WM_MBUTTONUP: case WM_MBUTTONDOWN: Key = OS_Key_MouseMiddle; break;
                case WM_RBUTTONUP: case WM_RBUTTONDOWN: Key = OS_Key_MouseRight; break;
            }
            
            Event = OS_MakeEvent(W32_OS.PermanentArena, &W32_OS.Events);
            Event->Kind = Kind;
            Event->Key = Key;
            Event->Position = W32_OS.MousePosition;
        }break;
        
        case WM_MOUSEHWHEEL: scroll_axis = Axis2_X; goto scroll;
        case WM_MOUSEWHEEL:
        scroll:;
        {
            i16 WheelDelta = HIWORD(WParam);
            Event = OS_MakeEvent(W32_OS.PermanentArena, &W32_OS.Events);
            Event->Kind = OS_EventKind_MouseScroll;
            Event->Scroll.Elements[scroll_axis] = (r32)WheelDelta;
        }break;
        
        case WM_SETCURSOR:
        {
            if (LOWORD(LParam) == HTCLIENT)
            {
                SetCursor(NULL);
                Result = TRUE;
            }
        }break;
        
        case WM_SYSKEYDOWN: case WM_SYSKEYUP:
        {
            DefWindowProcW(HWnd, Message, WParam, LParam);
        } fallthrough;
        case WM_KEYDOWN: case WM_KEYUP:
        {
            //b32 WasDown = !!(LParam & (1 << 30));
            b32 IsDown = !(LParam & (1 << 31));
            switch(WParam)
            {
                case VK_ESCAPE:   
                {
                    if (IsDown)
                    {
                        W32_GameInput.Escape.Released = false; 
                        W32_GameInput.Escape.Pressed  = true; 
                    }
                    else
                    {
                        W32_GameInput.Escape.Released = true; 
                        W32_GameInput.Escape.Pressed  = false; 
                    }
                } break;
                case VK_RETURN: 
                {
                    if (IsDown)
                    {
                        W32_GameInput.Confirm.Released = false; 
                        W32_GameInput.Confirm.Pressed  = true; 
                    }
                    else
                    {
                        W32_GameInput.Confirm.Released = true; 
                        W32_GameInput.Confirm.Pressed  = false; 
                    }
                } break;
                case VK_DOWN:
                {
                    if (IsDown)
                    {
                        W32_GameInput.Down.Released = false; 
                        W32_GameInput.Down.Pressed  = true; 
                    }
                    else
                    {
                        W32_GameInput.Down.Released = true; 
                        W32_GameInput.Down.Pressed  = false; 
                    }
                } break;
                case VK_UP:  
                {
                    if (IsDown)
                    {
                        W32_GameInput.Up.Released = false; 
                        W32_GameInput.Up.Pressed  = true; 
                    }
                    else
                    {
                        W32_GameInput.Up.Released = true; 
                        W32_GameInput.Up.Pressed  = false; 
                    }
                } break;
                case VK_LEFT:
                {
                    if (IsDown)
                    {
                        W32_GameInput.Left.Released = false; 
                        W32_GameInput.Left.Pressed  = true; 
                    }
                    else
                    {
                        W32_GameInput.Left.Released = true; 
                        W32_GameInput.Left.Pressed  = false; 
                    }
                } break;
                case VK_RIGHT:
                {
                    if (IsDown)
                    {
                        W32_GameInput.Right.Released = false; 
                        W32_GameInput.Right.Pressed  = true; 
                    }
                    else
                    {
                        W32_GameInput.Right.Released = true; 
                        W32_GameInput.Right.Pressed  = false; 
                    }
                } break;
                case 'O':
                {
                    if (IsDown)
                    {
                        W32_GameInput.O.Released = false; 
                        W32_GameInput.O.Pressed  = true; 
                    }
                    else
                    {
                        W32_GameInput.O.Released = true; 
                        W32_GameInput.O.Pressed  = false; 
                    }
                } break;
                case 'I':
                {
                    if (IsDown)
                    {
                        W32_GameInput.I.Released = false;
                        W32_GameInput.I.Pressed  = true; 
                    }
                    else
                    {
                        W32_GameInput.I.Released = true; 
                        W32_GameInput.I.Pressed  = false; 
                    }
                } break;
                
                default: break;
            }
            
#if 0            
            os_event_kind Kind = IsDown ? OS_EventKind_Press : OS_EventKind_Release;
            local_persist os_key key_table[256] = {0};
            local_persist b32 key_table_initialized = 0;
            if(!key_table_initialized)
            {
                key_table_initialized = 1;
                
                for (u32 i = 'A', j = OS_Key_A; i <= 'Z'; i += 1, j += 1)
                {
                    key_table[i] = (os_key)j;
                }
                for (u32 i = '0', j = OS_Key_0; i <= '9'; i += 1, j += 1)
                {
                    key_table[i] = (os_key)j;
                }
                for (u32 i = VK_F1, j = OS_Key_F1; i <= VK_F24; i += 1, j += 1)
                {
                    key_table[i] = (os_key)j;
                }
                
                key_table[VK_ESCAPE]        = OS_Key_Esc;
                key_table[VK_OEM_3]         = OS_Key_GraveAccent;
                key_table[VK_OEM_MINUS]     = OS_Key_Minus;
                key_table[VK_OEM_PLUS]      = OS_Key_Equal;
                key_table[VK_BACK]          = OS_Key_Backspace;
                key_table[VK_TAB]           = OS_Key_Tab;
                key_table[VK_SPACE]         = OS_Key_Space;
                key_table[VK_RETURN]        = OS_Key_Enter;
                key_table[VK_CONTROL]       = OS_Key_Ctrl;
                key_table[VK_SHIFT]         = OS_Key_Shift;
                key_table[VK_MENU]          = OS_Key_Alt;
                key_table[VK_UP]            = OS_Key_Up;
                key_table[VK_LEFT]          = OS_Key_Left;
                key_table[VK_DOWN]          = OS_Key_Down;
                key_table[VK_RIGHT]         = OS_Key_Right;
                key_table[VK_DELETE]        = OS_Key_Delete;
                key_table[VK_PRIOR]         = OS_Key_PageUp;
                key_table[VK_NEXT]          = OS_Key_PageDown;
                key_table[VK_HOME]          = OS_Key_Home;
                key_table[VK_END]           = OS_Key_End;
                key_table[VK_OEM_2]         = OS_Key_ForwardSlash;
                key_table[VK_OEM_PERIOD]    = OS_Key_Period;
                key_table[VK_OEM_COMMA]     = OS_Key_Comma;
                key_table[VK_OEM_7]         = OS_Key_Quote;
                key_table[VK_OEM_4]         = OS_Key_LeftBracket;
                key_table[VK_OEM_6]         = OS_Key_RightBracket;
            }
            
            os_key Key = OS_Key_Null;
            if(WParam < ArrayCount(key_table))
            {
                Key = key_table[WParam];
            }
            
            Event = OS_MakeEvent(W32_OS.PermanentArena, &W32_OS.Events);
            Event->Kind = Kind;
            Event->Key = Key;
#endif
            
        } break;
        
        case WM_MOUSEMOVE:
        {
            W32_OS.MousePosition.X = (r32)GET_X_LPARAM(LParam); 
            W32_OS.MousePosition.Y = (r32)GET_Y_LPARAM(LParam); 
        } break;
        
        case WM_SYSCOMMAND:
        {
            switch (WParam)
            {
                case SC_CLOSE:
                {
                    W32_OS.Quit = true;
                } break;
                case SC_KEYMENU:
                {}break;
                
                default:
                {
                    Result = DefWindowProcW(HWnd, Message, WParam, LParam);
                }break;
            }
        }break;
        
        case WM_CHAR: case WM_SYSCHAR:
        {
            u32 char_input = (u32)WParam;
            if (char_input == '\r')
            {
                char_input = '\n';
            }
            if((char_input >= 32 && char_input != 127) || char_input == '\t' || char_input == '\n')
            {
                Event = OS_MakeEvent(W32_OS.PermanentArena, &W32_OS.Events);
                Event->Kind = OS_EventKind_Text;
                Event->Character = char_input;
            }
        }break;
        
        case WM_DPICHANGED:
        {
            //F32 new_dpi = (F32)WParam;
            Result = DefWindowProcW(HWnd, Message, WParam, LParam);
        }break;
        
    }
    
    if(Event)
    {
        Event->Modifiers = W32_GetModifiers();
        OS_EventList_PushEvent(&W32_OS.Events, Event);
    }
    
    return Result;
}

int
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int ShowCmd)
{
    // NOTE(fakhri): main thread context
    {
        W32ThreadContext = MakeThreadContext();
        SetThreadCtx(&W32ThreadContext);
    }
    
    W32_PermArena = M_ArenaAlloc(Gigabytes(1));
    M_ArenaSetAutoAlign(W32_PermArena, 16);
    
    {
        W32_InitalPath = OS_GetSystemPath(W32_PermArena, SystemPath_Current);
        W32_ExecutablePath = OS_GetSystemPath(W32_PermArena, SystemPath_Binary);
        W32_GameDLLPath = PushStr8F(W32_PermArena, "%.*s\\%s_code.dll", Str8Expand(W32_ExecutablePath), Application_Name);
        W32_GameTempDLLPath = PushStr8F(W32_PermArena, "%.*s\\temp_%s_code.dll", Str8Expand(W32_ExecutablePath), Application_Name);
        
        Log("DLL Path: %.*s", Str8Expand(W32_GameDLLPath));
        Log("temp DLL Path: %.*s", Str8Expand(W32_GameTempDLLPath));
    }
    
    //- NOTE(fakhri): Find refresh rate
    r32 RefreshRate = 60.f;
    {
        DEVMODEA device_mode = {0};
        if(EnumDisplaySettingsA(0, ENUM_CURRENT_SETTINGS, &device_mode))
        {
            RefreshRate = (float)device_mode.dmDisplayFrequency;
        }
    }
    
    //- NOTE(fakhri): Init OS State 
    {
        W32_OS.PermanentArena = W32_PermArena;
        W32_OS.TargetFramePerSeconds = RefreshRate;
        W32_OS.GameDeltaTimeForFrame = 1.0f / RefreshRate;
        for (u32 ArenaIndex = 0;
             ArenaIndex < ArrayCount(W32_OS.FrameArenas);
             ++ArenaIndex)
        {
            W32_OS.FrameArenas[ArenaIndex] = M_ArenaAlloc(Gigabytes(16));
            M_ArenaSetAutoAlign(W32_OS.FrameArenas[ArenaIndex], 16);
            
        }
    }
    
#define WorkersCount 8
    WorkQueue_SetupQueue(&GlobalWorkQueue, WorkersCount);
    
    // NOTE(fakhri): start worker threads
    for (u32 Index = 0;
         Index < WorkersCount;
         ++Index)
    {
        struct worker_info *WorkerInfo = PushStructZero(W32_PermArena, struct worker_info);
        
        WorkerInfo->WorkQueue = &GlobalWorkQueue;
        WorkerInfo->WorkerID = Index;
        
        CloseHandle(CreateThread(0, 0, WorkerThreadMain, WorkerInfo, 0, 0));
    }
    
    Log("Started worker threads");
    
    //- NOTE(fakhri): initialize window class
    WNDCLASSW window_class = {0};
    {
        window_class.style = CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc = W32_WindowProc;
        window_class.hInstance = Instance;
        window_class.lpszClassName = W32_GraphicalWindowClassName;
        window_class.hCursor = LoadCursor(0, IDC_ARROW);
        if(!RegisterClassW(&window_class))
        {
            LogError("Window class registration failure.");
            goto quit;
        }
    }
    
    
    //- NOTE(fakhri): create a window
    w32_window_handle = CreateWindowW(W32_GraphicalWindowClassName, W32_GraphicalWindowTitle,
                                      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                      W32_GraphicalWindowWidth,
                                      W32_GraphicalWindowHeight,
                                      0, 0, Instance, 0);
    
    if(!w32_window_handle)
    {
        LogError("Window creation failure.");
        goto quit;
    }
    
    //- NOTE(fakhri): OpenGL initialization
    {
        w32_device_context = GetDC(w32_window_handle);
        W32_InitOpenGL(Instance);
        gladLoadGL();
    }
    
    
    ShowWindow(w32_window_handle, ShowCmd);
    UpdateWindow(w32_window_handle);
    
    game_state *GameState = PushStructZero(W32_OS.PermanentArena, game_state); Assert(GameState);
    w32_game_code GameCode = ZERO_STRUCT;
    W32_GameCodeLoad(&GameCode, W32_GameDLLPath, W32_GameTempDLLPath);
    GameCode.GamePermanentLoad(&W32_OS, GameState);
    
    // NOTE(fakhri): init directory watcher for shader files hotreload
    directory_watcher ShadersWatcher = ZERO_STRUCT;
    if (!W32_BeginWatchDirectory(&ShadersWatcher, W32_OS.PermanentArena, Str8Lit("data/shaders/")))
    {
        InvalidPath;
    }
    
    for(;!W32_OS.Quit;)
    {
        M_ArenaClear(OS_FrameArena(&W32_OS));
        W32_TimerBeginFrame(&W32_Timer);
        
        // NOTE(fakhri): Windows events loop
        {
            MSG message;
            while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
        
        // NOTE(fakhri): Update window size
        {
            RECT client_rect;
            GetClientRect(w32_window_handle, &client_rect);
            W32_OS.WindowSize.X = client_rect.right - client_rect.left;
            W32_OS.WindowSize.Y = client_rect.bottom - client_rect.top;
            glViewport(0, 0, W32_OS.WindowSize.Width, W32_OS.WindowSize.Height);
        }
        
        // TODO(fakhri): get controller input
        
        // NOTE(fakhri): hot reload game code if needed
        {
            W32_GameCodeUpdate(&GameCode, &W32_OS, W32_GameDLLPath, W32_GameTempDLLPath);
        }
        
        
        // NOTE(fakhri): hotreload shaders if needed
        {
            m_temp Scratch = GetScratch(0, 0);
            string8 ChangedShaderName = W32_CheckDirectoryChanges(&ShadersWatcher, Scratch.Arena);
            if (ChangedShaderName.str)
            {
                GameCode.GameHotLoadShader(Scratch.Arena, ChangedShaderName, GameState);
            }
            ReleaseScratch(Scratch);
        }
        
        // NOTE(fakhri): Update The game
        {
            b32 PrevFullScreen = W32_OS.Fullscreen;
            
            GameCode.GameUpdateAndRender(GameState, &W32_OS, &W32_GameInput, W32_OS.GameDeltaTimeForFrame);
            
            // NOTE(fakhri): Update fullscreen if necessary
            if(PrevFullScreen != W32_OS.Fullscreen)
            {
                W32_ToggleFullscreen(w32_window_handle);
            }
            
            OS_EndFrame(&W32_OS);
            W32_OpenGLRefreshScreen();
            W32_TimerEndFrame(&W32_OS, &W32_Timer);
        }
        
    }
    
    
    quit:;
    return 0;
}