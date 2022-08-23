
internal void
InitRenderer(m_arena *Arena, game_state *GameState)
{
    GameState->ShadersHashtable = MakeHashtable(Arena);
    
    // NOTE(fakhri): load shaders
    for (shader_kind ShaderKind = ShaderKind_None;
         ShaderKind < ShaderKind_Count;
         ++ShaderKind)
    {
        LoadShader(Arena, GameState, ShaderKind);
    }
    
    // NOTE(fakhri): load fonts
    for (font_kind FontKind = FontKind_None + 1;
         FontKind < FontKind_Count;
         ++FontKind)
    {
        LoadFont(GameState, Arena, FontKind);
    }
    
    for (texture_kind TextureKind = TextureKind_None;
         TextureKind < TextureKind_Count;
         ++TextureKind)
    {
        LoadTexture(GameState, TextureKind);
    }
}


internal v3r32
ScreenCoordsFromWorldCoords(game_state *GameState, v3r32 WorldCoords)
{
    v3r32 Result;
    Result.XY = AddVec2(MultiplyVec2f(SubtractVec2(WorldCoords.XY, GameState->CameraMoveSpec.Position),
                                      GameState->PixelsPerMeter), 
                        MultiplyVec2f(GameState->CameraSize, 0.5f));
    Result.Y = GameState->CameraSize.Height - Result.Y;
    Result.Z = WorldCoords.Z;
    return Result;
}

internal v3r32
NormalizedScreenCoordsFromScreenCoorfs(game_state *GameState, v3r32 ScreenCoords)
{
    ScreenCoords.X /= GameState->CameraSize.Width;
    ScreenCoords.Y /= GameState->CameraSize.Height;
    return ScreenCoords;
}



#define Render_PushRequest(PushBuffer, T) ((T *)_Render_PushRequest(PushBuffer, sizeof(T)))

internal void *
_Render_PushRequest(push_buffer *PushBuffer, u32 Size)
{
    void *Result = 0;
    Assert(PushBuffer->Size + Size < PushBuffer->Capacity);
    if (PushBuffer->Size + Size < PushBuffer->Capacity)
    {
        Result = (u8 *)PushBuffer->Memory + PushBuffer->Size;
        PushBuffer->Size += Size;
        ++PushBuffer->RequestsCount;
    }
    return Result;
}

internal void
Render_ClearBuffer(push_buffer *PushBuffer)
{
    PushBuffer->Size = 0;
    PushBuffer->RequestsCount = 0;
};

internal void
Render_PushClear(game_state *GameState, v4r32 Color)
{
    // NOTE(fakhri): clear all render request that came before this
    Render_ClearBuffer(GameState->PushBuffer);
    render_request_clear *ClearRequest = Render_PushRequest(GameState->PushBuffer, render_request_clear);
    ClearRequest->Header.Kind = RenderKind_Clear;
    ClearRequest->Header.Z = -MAX_Z;
    ClearRequest->Header.OffsetToNext = sizeof(render_request_clear);
    MemoryCopyStruct(&ClearRequest->Color, &Color);
}

internal void
Render_PushRectangle(game_state *GameState, v3r32 Pos, v2r32 Size, v4r32 Color, v3r32 Orientation)
{
    Pos = ScreenCoordsFromWorldCoords(GameState, Pos);
    render_request_rectangle *RectangleRequest = Render_PushRequest(GameState->PushBuffer, render_request_rectangle);
    RectangleRequest->Header.Kind = RenderKind_Rectangle;
    RectangleRequest->Header.Z = Pos.Z;
    RectangleRequest->Header.OffsetToNext = sizeof(render_request_rectangle);
    RectangleRequest->ScreenCoords = Pos.XY;
    RectangleRequest->Size = MultiplyVec2f(Size, GameState->PixelsPerMeter);
    MemoryCopyStruct(&RectangleRequest->Color, &Color);
    MemoryCopyStruct(&RectangleRequest->Orientation, &Orientation);
}

internal void
Render_PushQuad(game_state *GameState, v2r32 A, v2r32 B, v2r32 C, v2r32 D, r32 Z, v4r32 Color)
{
    A = ScreenCoordsFromWorldCoords(GameState, Vec3v(A, 0)).XY;
    B = ScreenCoordsFromWorldCoords(GameState, Vec3v(B, 0)).XY;
    C = ScreenCoordsFromWorldCoords(GameState, Vec3v(C, 0)).XY;
    D = ScreenCoordsFromWorldCoords(GameState, Vec3v(D, 0)).XY;
    
    render_request_quad *QuadRequest = Render_PushRequest(GameState->PushBuffer, render_request_quad);
    QuadRequest->Header.Kind = RenderKind_Quad;
    QuadRequest->Header.Z = Z;
    QuadRequest->Header.OffsetToNext = sizeof(render_request_quad);
    
    QuadRequest->A = A;
    QuadRequest->B = B;
    QuadRequest->C = C;
    QuadRequest->D = D;
    MemoryCopyStruct(&QuadRequest->Color, &Color);
}

internal void
Render_PushImage(game_state *GameState, texture_2d Texture,
                 v3r32 Pos, v2r32 Size, b32 FlipY,
                 v3r32 Orientation, b32 IsSizeInMeter, v4r32 Color, v4r32 Source)
{
    Pos = ScreenCoordsFromWorldCoords(GameState, Pos);
    render_request_image *ImageRequest = Render_PushRequest(GameState->PushBuffer, render_request_image);
    
    ImageRequest->Header.Kind = RenderKind_Image;
    ImageRequest->Header.Z = Pos.Z;
    ImageRequest->Header.OffsetToNext = sizeof(render_request_image);
    ImageRequest->ScreenCoords = Pos.XY;
    ImageRequest->Texture = Texture;
    ImageRequest->FlipY = FlipY;
    ImageRequest->Size = IsSizeInMeter ? MultiplyVec2f(Size, GameState->PixelsPerMeter) : Size;
    ImageRequest->Color = Color;
    ImageRequest->Source = Source;
    MemoryCopyStruct(&ImageRequest->Color, &Color);
    MemoryCopyStruct(&ImageRequest->Source, &Source);
    MemoryCopyStruct(&ImageRequest->Orientation, &Orientation);
}

internal r32
Render_PushText(game_state *GameState, string8 text, v3r32 Pos, v4r32 Color, font_kind FontToUse)
{
    Pos = ScreenCoordsFromWorldCoords(GameState, Pos);
    Assert(FontToUse < FontKind_Count && FontToUse > FontKind_None);
    
    Pos.X -= 0.5f * GetFontWidth(GameState, FontToUse, text);
    
    font *Font = GameState->Fonts + FontToUse;
    Assert(Font);
    
    v2r32 curr_point = Pos.XY;
    // NOTE(fakhri): render each character
    {
        for (u32 ch_index = 0;
             ch_index < text.size;
             ++ch_index)
        {
            u8 ch = text.str[ch_index];
            Assert(Font->MapFirst <= ch && ch < Font->MapOPL);
            u32 glyph_index = ch - Font->MapFirst;
            glyph *Glyph = Font->Map + glyph_index;
            
            v3r32 Glyph_Pos = Vec3(curr_point.X + 0.5f * Glyph->Size.X + Glyph->Offset.X, 
                                   curr_point.Y + 0.5f * Glyph->Size.Y + Glyph->Offset.Y,
                                   Pos.Z);
            Render_PushImage(GameState, Font->Texture, 
                             Glyph_Pos, Glyph->Size, 
                             0,
                             Vec3(0, 0, 0), 0, Color, Glyph->Source.CompactRect);
            
            curr_point.X += Glyph->Advance;
        }
    }
    
    return curr_point.X;
}


internal void
RenderLine(game_state *GameState, v2r32 A, v2r32 B, r32 Z, r32 HalfWidth, v4r32 Color)
{
    v2r32 AB = SubtractVec2(B, A);
    v2r32 N = Cross(Vec3(0, 0, 1), NormalizeVec3(Vec3v(AB, 0))).XY;
    
    v2r32 Diamter = MultiplyVec2f(N, HalfWidth);
    
    v2r32 AA = AddVec2(A, Diamter);
    v2r32 DD = SubtractVec2(A, Diamter);
    v2r32 BB = AddVec2(B, Diamter);
    v2r32 CC = SubtractVec2(B, Diamter);
    
    Render_PushQuad(GameState, AA, BB, CC, DD, Z, Color);
}


internal push_buffer *
AllocatePushBuffer(m_arena *Arena)
{
#define PUSH_BUFFER_SIZE Megabytes(64)
    push_buffer *PushBuffer = (push_buffer *)M_ArenaPushAligned(Arena, PUSH_BUFFER_SIZE, 16);
    
    PushBuffer->Memory = (u8 *)PushBuffer + sizeof(push_buffer);
    PushBuffer->Capacity = PUSH_BUFFER_SIZE - sizeof(push_buffer);
    PushBuffer->Size = 0;
    
    return PushBuffer;
}

internal void
Render_Begin(game_state *GameState, m_arena *FrameArena, v2r32 WindowSize)
{
    GameState->FrameArena = FrameArena;
    
    if (!EqualsVec2(WindowSize, GameState->CameraSize))
    {
        GameState->CameraSize = WindowSize;
    }
    
    GameState->PixelsPerMeter = GameState->CameraSize.Width / GameState->MetersInScreenWidth;
    
    GameState->PushBuffer = AllocatePushBuffer(FrameArena);
    Assert(GameState->PushBuffer);
}

internal render_sort_buffer
MakeSortBuffer(m_arena *Arena, u32 Count)
{
    render_sort_buffer SortBuffer;
    SortBuffer.Elements = PushArray(Arena, render_sort_entry, Count);
    SortBuffer.Count = Count;
    return SortBuffer;
};

internal void
SortRenderRequests(m_arena *Arena, render_sort_buffer *SortBuffer)
{
    for (u32 i = 0;
         i < SortBuffer->Count;
         ++i)
    {
        r32 min_z = SortBuffer->Elements[i].Z;
        u32 index_min = i;
        for (u32 j = i + 1;
             j < SortBuffer->Count;
             ++j)
        {
            r32 z_j = SortBuffer->Elements[j].Z;
            if (z_j < min_z)
            {
                min_z = z_j;
                index_min = j;
            }
        }
        
        render_sort_entry tmp = SortBuffer->Elements[i];
        SortBuffer->Elements[i] = SortBuffer->Elements[index_min];
        SortBuffer->Elements[index_min] = tmp;
    }
}

internal void
Render_End(game_state *GameState)
{
    m4r32 ortho_projection = Orthographic(0.0f, GameState->CameraSize.Width, 
                                          GameState->CameraSize.Height, 0.0f,
                                          -MAX_Z, MAX_Z);
    
    push_buffer *PushBuffer = GameState->PushBuffer;
    render_sort_buffer sort_buffer = MakeSortBuffer(GameState->FrameArena, PushBuffer->RequestsCount);
    
    for(u32 offset = 0, entry_index = 0;
        offset < PushBuffer->Size, entry_index < sort_buffer.Count;
        ++entry_index)
    {
        render_sort_entry *sort_entry = sort_buffer.Elements + entry_index;
        render_request_header *header = (render_request_header *)((u8 *)PushBuffer->Memory + offset);
        sort_entry->Z = header->Z;
        sort_entry->Offset = offset;
        offset += header->OffsetToNext;
        ++sort_entry;
    }
    
    SortRenderRequests(GameState->FrameArena, &sort_buffer);
    
    for(u32 entry_index = 0;
        entry_index < sort_buffer.Count;
        ++entry_index)
    {
        render_sort_entry *sort_entry = sort_buffer.Elements + entry_index;
        render_request_header *header = (render_request_header *)((u8 *)PushBuffer->Memory + sort_entry->Offset);
        switch(header->Kind)
        {
            case RenderKind_Rectangle:
            {
                render_request_rectangle *RectangleRequest = (render_request_rectangle *)header;
                v3r32 position = Vec3v(RectangleRequest->ScreenCoords, header->Z);
                m4r32 trans = Translate(position);
                m4r32 scale = ScaleBy(Vec3v(RectangleRequest->Size, 1.0f));
                m4r32 rotat = MultiplyMat4(Rotate(RectangleRequest->Orientation.X, Vec3(1,0,0)), 
                                           MultiplyMat4(Rotate(RectangleRequest->Orientation.Z, Vec3(0,0,1)),
                                                        Rotate(RectangleRequest->Orientation.Y, Vec3(0,1,0))));
                
                m4r32 model = MultiplyMat4(trans, MultiplyMat4(rotat, scale));
                
                shader_program *Program = GameState->Shaders + ShaderKind_Rectangle;
                OpenGL_DrawSolidRectangle(Program, ortho_projection, model, RectangleRequest->Color);
                
            } break;
            
            case RenderKind_Quad:
            {
                render_request_quad *QuadRequest = (render_request_quad *)header;
                shader_program *Program = GameState->Shaders + ShaderKind_Quad;
                OpenGL_DrawQuad(Program,QuadRequest->A, QuadRequest->B, QuadRequest->C, QuadRequest->D, ortho_projection, QuadRequest->Color);
                
            } break;
            
            case RenderKind_Image:
            {
                render_request_image *ImageRequest = (render_request_image *)header;
                
                v3r32 position = Vec3v(ImageRequest->ScreenCoords, header->Z);
                m4r32 trans = Translate(position);
                m4r32 scale = ScaleBy(Vec3v(ImageRequest->Size, 1.0f));
                m4r32 rotat = MultiplyMat4(Rotate(ImageRequest->Orientation.X, Vec3(1,0,0)), 
                                           MultiplyMat4(Rotate(ImageRequest->Orientation.Z, Vec3(0,0,1)),
                                                        Rotate(ImageRequest->Orientation.Y, Vec3(0,1,0))));
                
                m4r32 model = MultiplyMat4(trans, MultiplyMat4(rotat, scale));
                
                
                shader_program *Program = GameState->Shaders + ShaderKind_Texture;
                OpenGL_DrawImage(Program, ImageRequest->Texture, ImageRequest->FlipY, ortho_projection, model, ImageRequest->Source, ImageRequest->Color);
                
            } break;
            
            case RenderKind_Clear:
            {
                render_request_clear *ClearRequest = (render_request_clear *)header;
                OpenGL_Clear(ClearRequest->Color);
            } break;
            
            default :
            {
                Assert(header->Kind < RenderKind_Count);
                NotImplemented;
            } break;
        }
        
        glUseProgram(0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
