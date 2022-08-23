/* date = August 19th 2022 3:25 pm */

#ifndef GAME_SHADERS_H
#define GAME_SHADERS_H

typedef GLuint shader_id;
typedef GLuint vertex_array_id;
typedef GLuint vertex_buffer_id;

typedef u32 shader_kind;
enum
{
    ShaderKind_None,
    ShaderKind_Rectangle,
    ShaderKind_Texture,
    ShaderKind_Quad,
    
    ShaderKind_Count
};

typedef struct compile_shader_result
{
    b32 IsValid;
    shader_id ProgramID;
} compile_shader_result;

typedef struct shader_program
{
    b32 FirstLoad;
    
    shader_id ID;
    vertex_array_id  VAO;
    vertex_buffer_id VBO;
} shader_program;

#endif //GAME_SHADERS_H
