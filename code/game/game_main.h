/* date = August 19th 2022 3:21 pm */

#ifndef GAME_MAIN_H
#define GAME_MAIN_H

global thread_ctx GameThreadCtx;

typedef struct input_key
{
    b32 Pressed;
    b32 Released;
} input_key;

typedef struct game_input
{
    input_key Left;
    input_key Right;
    input_key Up;
    input_key Down;
    
    input_key O;
    input_key I;
    
    input_key Confirm;
    input_key Escape;
} game_input;

typedef struct move_spec
{
    v2r32 Force;
    v2r32 Position;
    v2r32 Velocity;
    r32   Mass;
    r32   SpringFactor;
    r32   FrictionFactor;
    r32  L0;
} move_spec;

typedef struct game_state
{
    r32 Time;
    m_arena *FrameArena;
    
    hashtable ShadersHashtable;
    shader_program Shaders[ShaderKind_Count];
    texture_2d Textures[TextureKind_Count];
    font Fonts[FontKind_Count];
    font_kind active_font;
    push_buffer *PushBuffer;
    
    r32 PixelsPerMeter;
    r32 MetersInScreenWidth;
    r32 TargetMetersInScreenWidth;
    r32 StartingMetersInScreenWidth;
    r32 ZoomTransitionDuration;
    r32 ZoomTransitionTime;
    b32 IsZooming;
    
    
    
    move_spec PlayerPoints[20];
    move_spec PlayerMoveSpec;
    move_spec CameraMoveSpec;
    
    v2r32 PlayerSize;
    
    b32 CameraFollowingPlayer;
    v2r32 CameraSize;
    
    v2r32 StationaryObject;
} game_state;


typedef void game_update_and_render_callback(game_state *, os_state *, game_input *, r32);
typedef void permanent_load_callbck(os_state *, game_state *);
typedef void hotload_callback(os_state *);
typedef void hotunload_callback();
typedef void hotload_shader_callback(m_arena *Arena, string8 ShaderName, game_state *GameState);


//~ NOTE(fakhri): stubs
internal void  GameUpdateAndRenderStub(game_state *GameState, os_state *OS_State, game_input *GameInput, r32 dt) {}
internal void GamePermanentLoadStub(os_state *OS_State, game_state *GameState){}
internal void GameHotLoadStub(os_state *OS_State){}
internal void GameHotUnLoadStub(){}
internal void GameHotLoadShaderStub(m_arena *Arena, string8 ShaderName, game_state *GameState) {}
#endif //GAME_MAIN_H
