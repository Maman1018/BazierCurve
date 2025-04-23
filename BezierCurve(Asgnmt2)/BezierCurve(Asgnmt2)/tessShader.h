#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Helper to load the source code from a file
inline std::string LoadShaderSource(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper to compile a shader and check for errors
inline GLuint CompileShader(const std::string& source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Error checking
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }

    return shader;
}

// Loads and links shaders into a program
inline GLuint LoadShaders(const std::string& vertexPath,
    const std::string& fragmentPath,
    const std::string& tessControlPath = "",
    const std::string& tessEvalPath = "") {
    // Load sources
    std::string vertexSource = LoadShaderSource(vertexPath);
    std::string fragmentSource = LoadShaderSource(fragmentPath);
    std::string tessControlSource = tessControlPath.empty() ? "" : LoadShaderSource(tessControlPath);
    std::string tessEvalSource = tessEvalPath.empty() ? "" : LoadShaderSource(tessEvalPath);

    // Compile shaders
    GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

    GLuint tessControlShader = 0;
    GLuint tessEvalShader = 0;

    if (!tessControlSource.empty()) {
        tessControlShader = CompileShader(tessControlSource, GL_TESS_CONTROL_SHADER);
    }

    if (!tessEvalSource.empty()) {
        tessEvalShader = CompileShader(tessEvalSource, GL_TESS_EVALUATION_SHADER);
    }

    // Link shaders into a program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    if (tessControlShader) glAttachShader(program, tessControlShader);
    if (tessEvalShader) glAttachShader(program, tessEvalShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Error checking
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << std::endl;
    }

    // Cleanup shaders after linking
    glDeleteShader(vertexShader);
    if (tessControlShader) glDeleteShader(tessControlShader);
    if (tessEvalShader) glDeleteShader(tessEvalShader);
    glDeleteShader(fragmentShader);

    return program; // Return the linked shader program
}