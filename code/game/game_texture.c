
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

read_only global string8
TexturePaths[TextureKind_Count] = 
{
    [TextureKind_None] = Str8LitComp(""),
};

internal texture_format_2d
TextureFormatFromInternalFormat(GLuint Format)
{
    texture_format_2d Result = TextureFormat2D_Null;
    switch(Format)
    {
        default: break;
        case GL_RED:  Result = TextureFormat2D_R8; break;
        case GL_RGB:  Result = TextureFormat2D_RGB8; break;
        case GL_RGBA: Result = TextureFormat2D_RGBA8; break;
    }
    return Result;
}

internal GLint
GenericFormatFromTextureFormat2D(texture_format_2d Format)
{
    GLint Result = 0;
    switch(Format)
    {
        default: break;
        case TextureFormat2D_R8:    {Result = GL_RED;  break;}
        case TextureFormat2D_RGB8:  {Result = GL_RGB;  break;}
        case TextureFormat2D_RGBA8: {Result = GL_RGBA; break;}
    }
    return Result;
}

internal GLenum
BaseTypeFromTextureFormat2D(texture_format_2d Format)
{
    GLint Result = 0;
    switch(Format)
    {
        default: break;
        case TextureFormat2D_R8:    {Result = GL_UNSIGNED_BYTE;  break; }
        case TextureFormat2D_RGB8:  {Result = GL_UNSIGNED_BYTE;  break; }
        case TextureFormat2D_RGBA8: {Result = GL_UNSIGNED_BYTE; break; }
    }
    return Result;
}

internal GLint
InternalFormatFromTextureFormat2D(texture_format_2d Format)
{
    GLint Result = 0;
    switch(Format)
    {
        default: break;
        case TextureFormat2D_R8:    {Result = GL_R8;  break; }
        case TextureFormat2D_RGB8:  {Result = GL_RGB8;  break; }
        case TextureFormat2D_RGBA8: {Result = GL_RGBA8; break; }
    }
    return Result;
}

internal texture_2d
ReserveTexture2D(v2i32 Size, texture_format_2d Format)
{
    texture_2d Texture = ZERO_STRUCT;
    glGenTextures(1, &Texture.ID);
    switch(Format)
    {
        default: break;
        case TextureFormat2D_R8:
        {
            glBindTexture(GL_TEXTURE_2D, Texture.ID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
            glBindTexture(GL_TEXTURE_2D, 0);
        }break;
    }
    
    Texture.Format = Format;
    Texture.Size = Size;
    return Texture;
}

internal void
FillTexture2D(texture_2d Texture, v2i32 Position, v2i32 Size, string8 Data)
{
    glBindTexture(GL_TEXTURE_2D, Texture.ID);
    GLenum GenericFormat = GenericFormatFromTextureFormat2D(Texture.Format);
    GLenum BaseType = BaseTypeFromTextureFormat2D(Texture.Format);
    glTexImage2D(GL_TEXTURE_2D, 0, GenericFormat, Size.Width, Size.Height, 0, GenericFormat, BaseType, Data.str);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

internal void
Releasetexture_2d(texture_2d Texture)
{
    glDeleteTextures(1, &Texture.ID);
}

internal void
LoadTexture(game_state *GameState, texture_kind TextureKind)
{
    Assert(TextureKind < TextureKind_Count);
    if (TextureKind == TextureKind_None) return;
    string8 TexturePath = TexturePaths[TextureKind];
    texture_2d *Texture = GameState->Textures + TextureKind;
    
    glGenTextures(1, &Texture->ID);
    glBindTexture(GL_TEXTURE_2D, Texture->ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    i32 nb_channels;
    void* Data = stbi_load(TexturePath.cstr, &Texture->Size.Width, &Texture->Size.Height, &nb_channels, 0);
    if (Data)
    {
        GLenum Format = 0;
        switch(nb_channels)
        {
            case 1: Format = GL_RED; break;
            case 3: Format = GL_RGB; break;
            case 4: Format = GL_RGBA; break;
            default: LogWarning("channels count %d not handled when loading image %s", nb_channels, TexturePath.str);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Texture->Size.Width, Texture->Size.Height, 0, Format, GL_UNSIGNED_BYTE, Data);
        stbi_image_free(Data);
        
        Texture->Format = TextureFormatFromInternalFormat(Format);
    }
    else
    {
        LogWarning("couldn't load image %.*s", Str8Expand(TexturePath));
    }
}
