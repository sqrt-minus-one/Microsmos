
#include "base/base_inc.h"
#include "os/os_inc.h"
#include "third_party/glad/glad.h" 
#include "third_party/wglext.h"
#include "game/game_inc.h"

#include "third_party/glad/glad.c"
#include "base/base_inc.c"
#include "os/os_inc.c"
#include "game/game_inc.c"

#define IdentityOrientation Vec3(0, 0, 0)

exported void
GameHotLoad(os_state *OS_State)
{
    // NOTE(fakhri): init the game thread context
    {
        GameThreadCtx = MakeThreadContext();
        SetThreadCtx(&GameThreadCtx);
    }
    gladLoadGL();
}

exported void
GameHotUnLoad()
{
    SetThreadCtx(0);
}


exported void
GamePermanentLoad(os_state *OS_State, game_state *GameState)
{
    GameHotLoad(OS_State);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    InitRenderer(OS_State->PermanentArena, GameState);
    
    GameState->MetersInScreenWidth = Meter(1.0f);
    GameState->ZoomTransitionDuration = 0.5f;
    GameState->TargetMetersInScreenWidth = GameState->MetersInScreenWidth;
    
    GameState->PlayerMoveSpec.Position = Vec2(0, 0);
    GameState->StationaryObject = Vec2(CentiMeter(10), 0);
    GameState->PlayerSize = Vec2(CentiMeter(1), CentiMeter(1));
    
    for (u32 Index = 0; Index < ArrayCount(GameState->PlayerPoints); ++Index)
    {
        move_spec *MoveSpec = GameState->PlayerPoints + Index;
        r32 Angle = (2 * PI32 * Index) / ArrayCount(GameState->PlayerPoints);
        MoveSpec->Position.X = CentiMeter(5.5f) * CosF(Angle);
        MoveSpec->Position.Y = CentiMeter(4.0f) * SinF(Angle);
        MoveSpec->L0 = LengthVec2(MoveSpec->Position);
        MoveSpec->FrictionFactor = 50.0f;
        MoveSpec->SpringFactor = 200.f;
        MoveSpec->Mass = 2.f;
    }
    
    move_spec *CameraMoveSpec = &GameState->CameraMoveSpec;
    CameraMoveSpec->FrictionFactor = 50.0f;
    CameraMoveSpec->SpringFactor = 70.f;
    CameraMoveSpec->Mass = 1;
    
    move_spec *PlayerMoveSpec = &GameState->PlayerMoveSpec;
    PlayerMoveSpec->FrictionFactor = 50.0f;
    PlayerMoveSpec->SpringFactor = 70.f;
    PlayerMoveSpec->Mass = 50;
}

internal void
UpdateMoveSpec(move_spec *MoveSpec, r32 dt, v2r32 Force)
{
#if 0
    // NOTE(fakhri): friction force
    MultiplyVec2f(MoveSpec->Velocity, -MoveSpec->FrictionFactor);
#endif
    v2r32 Acceleration = MultiplyVec2f(Force, 1.0f / MoveSpec->Mass);
    MoveSpec->Velocity = AddVec2(MoveSpec->Velocity, MultiplyVec2f(Acceleration, dt));
    MoveSpec->Position = AddVec2(MoveSpec->Position, 
                                 AddVec2(MultiplyVec2f(MoveSpec->Velocity, dt), 
                                         MultiplyVec2f(Acceleration, 0.5f * SQUARE(dt))));
}


exported void
GameUpdateAndRender(game_state *GameState, os_state *OS_State, game_input *GameInput, r32 dt)
{
    GameState->Time += dt;
    GameState->ZoomTransitionDuration = 0.4f;
    
    if (GameInput->O.Pressed)
    {
        GameInput->O.Pressed = false;
        GameState->IsZooming = 1;
        GameState->StartingMetersInScreenWidth = GameState->MetersInScreenWidth;
        GameState->TargetMetersInScreenWidth = GameState->MetersInScreenWidth + CentiMeter(10);
        GameState->ZoomTransitionTime = 0;
    }
    
    if (GameInput->I.Pressed)
    {
        GameInput->I.Pressed = false;
        GameState->IsZooming = 1;
        GameState->StartingMetersInScreenWidth = GameState->MetersInScreenWidth;
        GameState->TargetMetersInScreenWidth = GameState->MetersInScreenWidth -  CentiMeter(10.f);
        GameState->ZoomTransitionTime = 0;
        if (GameState->TargetMetersInScreenWidth < 0)
        {
            GameState->TargetMetersInScreenWidth = 0;
        }
    }
    
    if (GameState->IsZooming)
    {
        GameState->ZoomTransitionTime += dt;
        if (GameState->ZoomTransitionTime >= 1.0f)
        {
            GameState->IsZooming = 0;
        }
        r32 t = MapIntoRange01(0, GameState->ZoomTransitionTime, GameState->ZoomTransitionDuration);
        GameState->MetersInScreenWidth = (PowerF(GameState->StartingMetersInScreenWidth, 1.0f - t) * 
                                          PowerF(GameState->TargetMetersInScreenWidth,   t));
    }
    
    Render_Begin(GameState, OS_FrameArena(OS_State), Vec2FromVec2i32(OS_State->WindowSize));
    Render_PushClear(GameState, Vec4(0.2f, 0.2f, 0.2f, 1.f));
    
    // NOTE(fakhri): the player input force
    v2r32 MovementForce = ZERO_STRUCT;
    if (GameInput->Left.Pressed)
    {
        MovementForce.X += -1;
    }
    if (GameInput->Right.Pressed)
    {
        MovementForce.X += +1;
    }
    if (GameInput->Up.Pressed)
    {
        MovementForce.Y += +1;
    }
    if (GameInput->Down.Pressed)
    {
        MovementForce.Y += -1;
    }
    
    GameState->PlayerMoveSpec.Force = Vec2(0, 0);
    
    // NOTE(fakhri): update the player
    for (u32 Index = 0; Index < ArrayCount(GameState->PlayerPoints); ++Index)
    {
        move_spec *CurrPoint = GameState->PlayerPoints + Index;
        CurrPoint->Force = Vec2(0, 0);
        r32 IdealLength = MiliMeter(2.0f);
        
        // NOTE(fakhri): move the particle
        {
            v2r32 Segment = SubtractVec2(CurrPoint->Position, GameState->PlayerMoveSpec.Position);
            r32 Influence = DotVec2(NormalizeVec2(Segment), NormalizeVec2(MovementForce));
#if 1
            r32 Distribution = 0.6f;
            Influence = 1 + (1.0f / PI32) * (ATanF(69 * (Influence - Distribution)) - ATanF(69.f * (1.0f - Distribution)));
#else
            Influence = Clamp(0, SQUARE(Influence), 1);
#endif
            v2r32 Force = MultiplyVec2f(NormalizeVec2(MovementForce), 2 * Influence);
            CurrPoint->Force = AddVec2(CurrPoint->Force, Force);
        }
        
        {
            u32 PrevIndex = (Index + ArrayCount(GameState->PlayerPoints) - 1) % ArrayCount(GameState->PlayerPoints);
            move_spec *PrevPoint = GameState->PlayerPoints + PrevIndex;
            v2r32 Segment = SubtractVec2(PrevPoint->Position, CurrPoint->Position);
            r32 SegmentLength = LengthVec2(Segment);
            v2r32 Force = MultiplyVec2f(NormalizeVec2(Segment), -CurrPoint->SpringFactor * (IdealLength - SegmentLength));
            CurrPoint->Force = AddVec2(CurrPoint->Force, Force);
            RenderLine(GameState, PrevPoint->Position, CurrPoint->Position, 0, MiliMeter(1), Vec4(1, 0, 1, 1));
        }
        
        {
            u32 NextIndex = (Index + 1) % ArrayCount(GameState->PlayerPoints);
            move_spec *NextPoint = GameState->PlayerPoints + NextIndex;
            v2r32 Segment = SubtractVec2(NextPoint->Position, CurrPoint->Position);
            r32 SegmentLength = LengthVec2(Segment);
            v2r32 Force = MultiplyVec2f(NormalizeVec2(Segment), -CurrPoint->SpringFactor * (IdealLength - SegmentLength));
            CurrPoint->Force = AddVec2(CurrPoint->Force, Force);
            RenderLine(GameState, NextPoint->Position, CurrPoint->Position, 0, MiliMeter(1), Vec4(1, 0, 1, 1));
        }
        
        {
            v2r32 Segment = SubtractVec2(GameState->PlayerMoveSpec.Position, CurrPoint->Position);
            r32 SegmentLength = LengthVec2(Segment);
            v2r32 Force = MultiplyVec2f(NormalizeVec2(Segment), -GameState->PlayerMoveSpec.SpringFactor * (CurrPoint->L0 - SegmentLength));
            CurrPoint->Force = AddVec2(CurrPoint->Force, Force);
            GameState->PlayerMoveSpec.Force = AddVec2(GameState->PlayerMoveSpec.Force, MultiplyVec2f(Force, -1));
#if 0
            RenderLine(GameState, GameState->PlayerMoveSpec.Position, CurrPoint->Position, 0, MiliMeter(1), Vec4(1, 1, 0, 1));
#endif
        }
        
#if 0
        Render_PushRectangle(GameState, Vec3v(CurrPoint->Position, 0), Vec2(MiliMeter(5), MiliMeter(5)), Vec4(1, 1, 1, 1), IdentityOrientation);
#endif
    }
    
    
    for(u32 Index = 0; Index < ArrayCount(GameState->PlayerPoints); ++Index)
    {
        move_spec *CurrPoint = GameState->PlayerPoints + Index;
        CurrPoint->Force = AddVec2(CurrPoint->Force, MultiplyVec2f(CurrPoint->Velocity, -CurrPoint->FrictionFactor));
        UpdateMoveSpec(CurrPoint, dt, CurrPoint->Force);
    }
    
    GameState->PlayerMoveSpec.Force = AddVec2(GameState->PlayerMoveSpec.Force, MultiplyVec2f(GameState->PlayerMoveSpec.Velocity, -GameState->PlayerMoveSpec.FrictionFactor));
    // NOTE(fakhri): apply friction on the player
    UpdateMoveSpec(&GameState->PlayerMoveSpec, dt, GameState->PlayerMoveSpec.Force);
    
#if 1
    // NOTE(fakhri): render the player core
    Render_PushRectangle(GameState, Vec3v(GameState->PlayerMoveSpec.Position, 0), GameState->PlayerSize, Vec4(1.0f, 0.0f, 0.2f, 1.f), IdentityOrientation);
#endif
    
    
    // NOTE(fakhri): move the camera
    {
        if (!GameState->CameraFollowingPlayer && (LengthSquaredVec2(SubtractVec2(GameState->PlayerMoveSpec.Position, 
                                                                                 GameState->CameraMoveSpec.Position)) > CentiMeter(1)))
        {
            GameState->CameraFollowingPlayer = 1;
        }
        
        if (GameState->CameraFollowingPlayer)
        {
            v2r32 Force = MultiplyVec2f(SubtractVec2(GameState->PlayerMoveSpec.Position, GameState->CameraMoveSpec.Position), GameState->CameraMoveSpec.SpringFactor);
            Force = AddVec2(Force, MultiplyVec2f(GameState->CameraMoveSpec.Velocity, -GameState->CameraMoveSpec.FrictionFactor));
            UpdateMoveSpec(&GameState->CameraMoveSpec, dt, Force);
            
            if (LengthVec2(SubtractVec2(GameState->PlayerMoveSpec.Position, GameState->CameraMoveSpec.Position)) < MiliMeter(5.0f))
            {
                GameState->CameraFollowingPlayer = 0;
            }
        }
    }
    
#if 1
    Render_PushRectangle(GameState, Vec3v(GameState->StationaryObject, 0), GameState->PlayerSize, Vec4(1.0f, 0.6f, 0.2f, 1.f), IdentityOrientation);
#endif
    
    Render_End(GameState);
    
}