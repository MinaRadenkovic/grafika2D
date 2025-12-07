#include "../Header/Util.h";

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "../Header/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../Header/stb_image_resize2.h"


// Autor: Nedeljko Tesanovic
// Opis: pomocne funkcije za zaustavljanje programa, ucitavanje sejdera, tekstura i kursora
// Smeju se koristiti tokom izrade projekta

int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)

    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

//GLFWcursor* loadImageToCursor(const char* filePath) {
//    int TextureWidth;
//    int TextureHeight;
//    int TextureChannels;
//
//    std::cout << "Trying to load cursor from: " << filePath << std::endl;
//
//
//    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
//
//    if (ImageData != NULL)
//    {
//        GLFWimage image;
//        image.width = TextureWidth;
//        image.height = TextureHeight;
//        image.pixels = ImageData;
//
//        // Tacka na površini slike kursora koja se ponaša kao hitboks, moze se menjati po potrebi
//        // Trenutno je gornji levi ugao, odnosno na 20% visine i 20% sirine slike kursora
//        int hotspotX = TextureWidth / 5;
//        int hotspotY = TextureHeight / 5;
//
//        GLFWcursor* cursor = glfwCreateCursor(&image, hotspotX, hotspotY);
//        stbi_image_free(ImageData);
//        return cursor;
//    }
//    else {
//        std::cout << "Kursor nije ucitan! Putanja kursora: " << filePath << std::endl;
//        stbi_image_free(ImageData);
//
//    }
//}

GLFWcursor* loadImageToCursor(const char* filePath) {
    int width, height, channels;

    unsigned char* data = stbi_load(filePath, &width, &height, &channels, 4);
    if (!data) {
        std::cout << "Greska: ne mogu ucitati sliku kursora!" << std::endl;
        return nullptr;
    }

    const int targetW = 256;
    const int targetH = 256;

    unsigned char* resized = new unsigned char[targetW * targetH * 4];

    stbir_resize_uint8_linear(
        data, width, height, width * 4,
        resized, targetW, targetH, targetW * 4,
        STBIR_RGBA
    );


    stbi_image_free(data);

    GLFWimage image;
    image.width = targetW;
    image.height = targetH;
    image.pixels = resized;

    int hotspotX = targetW / 2;
    int hotspotY = targetH / 2;

    GLFWcursor* cursor = glfwCreateCursor(&image, hotspotX, hotspotY);

    //delete[] resized;

    return cursor;
}


void limitFramesPerSecond(double targetFps, double& lastTime)
{
    double targetFrameTime = 1.0 / targetFps;
    double current = glfwGetTime();
    double elapsed = current - lastTime;
    if (elapsed < targetFrameTime) {
        double sleepTime = targetFrameTime - elapsed;
        // sleep for the remaining time (convert to ms)
        auto sleepMs = std::chrono::duration<double>(sleepTime);
        std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(sleepMs));
        // busy-check small remainder
        while ((glfwGetTime() - lastTime) < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
    lastTime = glfwGetTime();
}

unsigned int compileShaderFromSource(GLenum type, const char* sourceCode)
{
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Compile error: " << infoLog << std::endl;
    }
    return shader;
}
