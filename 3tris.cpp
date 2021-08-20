#include <functional>
#include <iostream>
#include <emscripten.h>
#include <SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>

// Shader sources
const GLchar* vertexSource =
    "attribute vec4 position;                     \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.xyz, 1.0);     \n"
    "}                                            \n";
// ½¥±äÉ«shader
const GLchar* fragmentSource2 =
    "precision mediump float;\n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor[0] = gl_FragCoord.x/640.0;    \n"
    "  gl_FragColor[1] = gl_FragCoord.y/480.0;    \n"
    "  gl_FragColor[2] = 0.5;                     \n"
    "}                                            \n";

// 
const GLchar* fragmentSource1 =
"precision mediump float;\n"
"void main()                                  \n"
"{                                            \n"
"  gl_FragColor[0] = 1.0;    \n"
"  gl_FragColor[1] = 1.0;    \n"
"  gl_FragColor[2] = 0.5;                     \n"
"  gl_FragColor[3] = 0.5;                     \n"
"}                                            \n";



// an example of something we will control from the javascript side
bool background_is_black = true;

// the function called by the javascript code
extern "C" void EMSCRIPTEN_KEEPALIVE toggle_background_color() { background_is_black = !background_is_black; }

std::function<void()> loop;
void main_loop() { loop(); }

int main()
{
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, nullptr);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create a Vertex Buffer Object and copy the vertex data to it    

	GLuint vbo1, vbo2, vbo3, vbo4;
	GLuint vbo[4];
	{
		glGenBuffers(1, &vbo[0]);
		GLfloat vertices[] = { -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f };
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	{
		glGenBuffers(1, &vbo[1]);
		GLfloat vertices[] = { 0.5f, 0.5f, 0.0f, 0.0f,1.0f, 0.0f};
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);			
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	{
		glGenBuffers(1, &vbo[2]);
		GLfloat vertices[] = { -0.5f, 1.0f, -1.0f, 0.5f, 0.0f, 0.5f };
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	{
		glGenBuffers(1, &vbo[3]);
		GLfloat vertices[] = { 0.3f, 0.4f, 0.0f, -0.4f, 0.5f, -1.0f };
		glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
    

	// Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    GLuint fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &fragmentSource1, nullptr);
	glCompileShader(fragmentShader1);
	GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fragmentSource2, nullptr);
	glCompileShader(fragmentShader2);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram1, shaderProgram2;
	GLuint shaderProgram[4];

    auto lamb_createAndAttatch = [&](GLuint vertexShader, GLuint fragmentShader){
		GLuint shaderProgram = glCreateProgram();
		// test shader link
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		
        return shaderProgram;
    };
    
	
    shaderProgram[0] = lamb_createAndAttatch(vertexShader,  fragmentShader1);    
    shaderProgram[1] = lamb_createAndAttatch(vertexShader,  fragmentShader2);
	shaderProgram[2] = lamb_createAndAttatch(vertexShader, fragmentShader1);
	shaderProgram[3] = lamb_createAndAttatch(vertexShader, fragmentShader2);
    
	

    

    loop = [&]
    {  
        // Clear the screen
        if( background_is_black )
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        else
            glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < 4; ++i)
        {
			glLinkProgram(shaderProgram[i]);
			glUseProgram(shaderProgram[i]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
			glEnableVertexAttribArray(0);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
        }       

        SDL_GL_SwapWindow(window);
    };

    emscripten_set_main_loop(main_loop, 0, true);

    return EXIT_SUCCESS;
}
