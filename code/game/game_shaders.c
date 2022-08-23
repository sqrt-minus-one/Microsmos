
internal compile_shader_result 
CompileShader(string8 ShaderName)
{
    compile_shader_result Result = ZERO_STRUCT;
    string8 ShaderPath = ZERO_STRUCT;
    
    m_temp Scratch = GetScratch(0, 0);
    ShaderPath = PushStr8F(Scratch.Arena, "data/shaders/%.*s", Str8Expand(ShaderName));
    
    string8 ShaderCode = OS_LoadEntireFile(Scratch.Arena, ShaderPath);
    if (ShaderCode.size == 0)
    {
        LogError("Couldn't open the shader file %.*s", Str8Expand(ShaderPath));
        goto end;
    }
    
    const char *VertexShaderCode[3] = {"#version 330 core\n","#define VERTEX_SHADER\n", ShaderCode.cstr};
    const char *FragmentShaderCode[3] = {"#version 330 core\n", "#define FRAGMENT_SHADER\n", ShaderCode.cstr};
    
    shader_id VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    shader_id FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    // NOTE(fakhri): compile vertex shader
    {
        glShaderSource(VertexShaderID, 3, VertexShaderCode, 0);
        glCompileShader(VertexShaderID);
        GLint success = 0;
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE)
        {
            char infoLog[512];
            glGetShaderInfoLog(VertexShaderID, sizeof(infoLog), 0, infoLog);
            LogError("Couldn't compile vertex shader program");
            LogError("shader error :\n%s", infoLog);
            glDeleteShader(VertexShaderID);
            goto end;
        }
    }
    
    // NOTE(fakhri): compiler fragment shader
    {
        glShaderSource(FragmentShaderID, 3, FragmentShaderCode, 0);
        glCompileShader(FragmentShaderID);
        GLint success = 0;
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE)
        {
            char infoLog[512];
            glGetShaderInfoLog(FragmentShaderID, sizeof(infoLog), 0, infoLog);
            LogError("Couldn't compile fragment shader program");
            LogError("shader error :\n%s", infoLog);
            glDeleteShader(FragmentShaderID);
            goto end;
        }
    }
    
    // NOTE(fakhri): link shader program
    {
        shader_id Program = glCreateProgram();
        glAttachShader(Program, VertexShaderID);
        glAttachShader(Program, FragmentShaderID);
        glLinkProgram(Program);
        GLint success = 0;
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (success == GL_TRUE)
        {
            Result.IsValid = true;
            glDetachShader(Program, VertexShaderID);
            glDetachShader(Program, FragmentShaderID);
            Result.ProgramID = Program;
        }
        else
        {
            char infoLog[512];
            glGetProgramInfoLog(Result.ProgramID, sizeof(infoLog), 0, infoLog);
            LogError("Couldn't link shader Program\n");
            LogError("shader error :\n%s", infoLog);
            glDeleteProgram(Result.ProgramID);
            goto end;
        }
    }
    
    if (Result.IsValid)
    {
        Log("Shader %s compiled successfully", ShaderPath.str); 
    }
    
    end:;
    ReleaseScratch(Scratch);
    return Result;
}

internal void
SetupShader(game_state *GameState, shader_kind Kind)
{
    local_persist r32 RectangleVertices[] = {
        // position
        -0.5f, -0.5f,
        -0.5f, +0.5f,
        +0.5f, +0.5f,
        
        -0.5f, -0.5f,
        +0.5f, +0.5f,
        +0.5f, -0.5f,
    };
    
    local_persist r32 TextureVertices[] = {
        // position         texture
        -0.5f, -0.5f, 	 0.0f, 0.0f,
        -0.5f, +0.5f, 	 0.0f, 1.0f,
        +0.5f, +0.5f, 	 1.0f, 1.0f,
        
        -0.5f, -0.5f, 	 0.0f, 0.0f,
        +0.5f, +0.5f, 	 1.0f, 1.0f,
        +0.5f, -0.5f, 	 1.0f, 0.0f,
    };
    
    shader_program *Program = GameState->Shaders + Kind;
    glUseProgram(Program->ID);
    
    // NOTE(fakhri): per shader kind init
    if (Program->FirstLoad)
    {
        // NOTE(fakhri): stuff that only need to happen the first time we load the shader
        Program->FirstLoad = false;
        
        // NOTE(fakhri): shared settings
        glGenVertexArrays(1, &Program->VAO);
        glBindVertexArray(Program->VAO);
        
        glGenBuffers(1, &Program->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, Program->VBO);
        
        switch(Kind)
        {
            case ShaderKind_None: break;
            case ShaderKind_Quad:
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(RectangleVertices), RectangleVertices, GL_DYNAMIC_DRAW);
                
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(r32), (void*)0);
                
            } break;
            case ShaderKind_Rectangle:
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(RectangleVertices), RectangleVertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(r32), (void*)0);
                
            } break;
            case ShaderKind_Texture:
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(TextureVertices), TextureVertices, GL_STATIC_DRAW);
                
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(r32), (void *)0);
            } break;
            default:
            {
                Assert(Kind < ShaderKind_Count);
                NotImplemented;
            } break;
        }
    }
    
    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

read_only global char *ShadersName[ShaderKind_Count] = {
    [ShaderKind_Rectangle]    = "rectangle_shader.glsl",
    [ShaderKind_Texture] = "texture_shader.glsl",
    [ShaderKind_Quad] = "quad_shader.glsl",
};

internal void
LoadShader(m_arena *Arena, game_state *GameState, shader_kind ShaderKind)
{
    if (ShaderKind == ShaderKind_None) return;
    
    Assert(ShadersName[ShaderKind]);
    string8 ShaderName = Str8C(ShadersName[ShaderKind]);
    
    // NOTE(fakhri): create new slot and add it to the hash table
    Hashtable_Insert(Arena, &GameState->ShadersHashtable, MakeHashtableKey(ShaderName), PtrFromInt(ShaderKind));
    
    // NOTE(fakhri): compile the shader
    compile_shader_result ShaderResult = CompileShader(ShaderName);
    if (ShaderResult.IsValid)
    {
        GameState->Shaders[ShaderKind].ID = ShaderResult.ProgramID;
        GameState->Shaders[ShaderKind].FirstLoad = 1;
        SetupShader(GameState, ShaderKind);
    }
    else
    {
        InvalidPath;
    }
}

exported void
GameHotLoadShader(m_arena *Arena, string8 ShaderName, game_state *GameState)
{
    hashtable_slot *Slot = Hashtable_Find(&GameState->ShadersHashtable, MakeHashtableKey(ShaderName));
    if (Slot)
    {
        shader_kind ShaderKind = (shader_kind)IntFromPtr(Slot->Value);
        compile_shader_result ShaderResult = CompileShader(ShaderName);
        if (ShaderResult.IsValid)
        {
            glDeleteProgram(GameState->Shaders[ShaderKind].ID);
            GameState->Shaders[ShaderKind].ID = ShaderResult.ProgramID;
            SetupShader(GameState, ShaderKind);
        }
    }
    else
    {
        Log("Make sure that you added %.*s to the hashtable, couldn't find it", Str8Expand(ShaderName));
    }
}
