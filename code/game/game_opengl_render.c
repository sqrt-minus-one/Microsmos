
internal void
OpenGL_DrawImage(shader_program *Program, texture_2d Texture, b32 flip_y, m4r32 projection, m4r32 model, v4r32 src, v4r32 color)
{
    glUseProgram(Program->ID);
    
    // NOTE(fakhri): sending uniforms to the GPU
    {
        glUniformMatrix4fv(glGetUniformLocation(Program->ID, "projection"), 1, GL_FALSE, (r32*)&projection);
        glUniformMatrix4fv(glGetUniformLocation(Program->ID, "model"), 1, GL_FALSE, (r32*)&model);
        glUniform4fv(glGetUniformLocation(Program->ID, "src"), 1, (r32 *)&src);
        glUniform4fv(glGetUniformLocation(Program->ID, "tilting_color"), 1, (r32 *)&color);
        glUniform1i(glGetUniformLocation(Program->ID, "flip_y"), flip_y);
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture.ID);
    glBindVertexArray(Program->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

internal void
OpenGL_DrawSolidRectangle(shader_program *Program, m4r32 projection, m4r32 model, v4r32 color)
{
    glUseProgram(Program->ID);
    
    // NOTE(fakhri): sending uniforms to the GPU
    {
        glUniformMatrix4fv(glGetUniformLocation(Program->ID, "projection"), 1, GL_FALSE, (r32*)&projection);
        glUniformMatrix4fv(glGetUniformLocation(Program->ID, "model"), 1, GL_FALSE, (r32*)&model);
        glUniform4fv(glGetUniformLocation(Program->ID, "color"), 1, (r32*)&color);
    }
    
    glBindVertexArray(Program->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

internal void
OpenGL_Clear(v4r32 color)
{
    glClearColor(color.R, color.G, color.B, color.A);
    glClear(GL_COLOR_BUFFER_BIT);
}


// NOTE(fakhri): A, B, C, D are the quad points clockwise in screen coords
internal void
OpenGL_DrawQuad(shader_program *Program, v2r32 A, v2r32 B, v2r32 C, v2r32 D, m4r32 Projection, v4r32 Color)
{
    glUseProgram(Program->ID);
    
    r32 Vertices[6][2] = {
        { A.X,     A.Y},
        { C.X,     C.Y},
        { B.X,     B.Y},
        
        { A.X,     A.Y},
        { D.X,     D.Y},
        { C.X,     C.Y},
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, Program->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // NOTE(fakhri): sending uniforms to the GPU
    {
        glUniformMatrix4fv(glGetUniformLocation(Program->ID, "projection"), 1, GL_FALSE, (r32*)&Projection);
        glUniform4fv(glGetUniformLocation(Program->ID, "color"), 1, (r32*)&Color);
    }
    
    glBindVertexArray(Program->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
