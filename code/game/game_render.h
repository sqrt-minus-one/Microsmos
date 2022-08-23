/* date = August 19th 2022 3:42 pm */

#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#define MAX_Z Meter(100)

typedef enum
{
    RenderKind_None,
    RenderKind_Rectangle,
    RenderKind_Quad,
    RenderKind_Image,
    RenderKind_Clear,
    
    RenderKind_Count,
} render_kind;

typedef struct render_request_header
{
    render_kind Kind;
    u32 OffsetToNext;
    r32 Z;
} render_request_header;

typedef struct render_request_rectangle
{
    render_request_header Header;
    v2r32 ScreenCoords;
    v2r32 Size;
    v4r32 Color;
    v3r32 Orientation;
} render_request_rectangle;

typedef struct render_request_quad
{
    render_request_header Header;
    v4r32 Color;
    v2r32 A;
    v2r32 B;
    v2r32 C;
    v2r32 D;
} render_request_quad;


typedef struct render_request_image
{
    render_request_header Header;
    v2r32 ScreenCoords;
    texture_2d Texture;
    b32 FlipY;
    v2r32 Size;
    v3r32 Orientation;
    v4r32 Color;
    v4r32 Source;
} render_request_image;

typedef struct render_request_clear
{
    render_request_header Header;
    v4r32 Color;
} render_request_clear;

typedef struct push_buffer
{
    void *Memory;
    u32 Capacity;
    u32 Size;
    u32 RequestsCount;
} push_buffer;

typedef struct render_sort_entry
{
    u32 Offset;
    r32 Z;
} render_sort_entry;

typedef struct render_sort_buffer
{
    render_sort_entry *Elements;
    u32 Count;
} render_sort_buffer;

#endif//GAME_RENDER_H
