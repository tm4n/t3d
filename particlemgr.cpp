#include "particlemgr.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *particlemgr::mVertexShaderCode = 
	"#version 110 \n"
	"#ifdef GL_ES \n"
	"precision mediump float; \n"
	"#endif \n"
			
    "uniform mat4 uMVPMatrix; \n"
	"uniform float Size; \n"
    "attribute vec4 vPosition; \n"
    "attribute vec2 TexCoordIn; \n" 
    "varying vec2 TexCoordOut; \n"
    "void main() { \n"
    // the matrix must be included as a modifier of gl_Position
    " gl_Position = uMVPMatrix *vPosition; \n"
    " TexCoordOut = TexCoordIn; \n" 
    "}";

const char *particlemgr::mFragmentShaderCode =
	"#version 110 \n"
	"#ifdef GL_ES \n"
	"precision mediump float; \n"
	"#endif \n"
			
    "varying vec2 TexCoordOut; \n" 
    "uniform sampler2D Texture; \n"
	"uniform float Alpha; \n"
    "void main() { \n" 
    //"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); \n"
	"  gl_FragColor = texture2D(Texture, TexCoordOut); \n" 
	"  gl_FragColor.w *= Alpha; \n"
	"}";

float particlemgr::squareCoords[] = { // in counterclockwise order:
        -1.0f,  1.0f, 0.0f,   // top left
       -1.0f, -1.0f, 0.0f,   // bottom left
        1.0f, -1.0f, 0.0f,    // bottom right
        -1.0f,  1.0f, 0.0f,   // top left
        1.0f, -1.0f, 0.0f,    // bottom right
        1.0f,  1.0f, 0.0f   // top left
};
	
float particlemgr::squareTexCoords[] = { // in counterclockwise order:
        0.0f,  0.0f,  // top left
       0.0f, 1.0f,   // bottom left
        1.0f, 1.0f,    // bottom right
        0.0f,  0.0f,   // top left
        1.0f, 1.0f,    // bottom right
        1.0f,  0.0f   // top right
};


particlemgr::particlemgr()
{

	// ready up draw shaders
	// initialize shaders
    int vertexShader = loadShader(GL_VERTEX_SHADER, mVertexShaderCode);
    int fragmentShader = loadShader(GL_FRAGMENT_SHADER, mFragmentShaderCode);

    mProgram = glCreateProgram();             // create empty OpenGL ES Program
    glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
    glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
    glLinkProgram(mProgram);                  // creates OpenGL ES program executables
        
    // get handle to fragment shader's alpha member
    mAlphaHandle = glGetUniformLocation(mProgram, "Alpha");
        
    // get handle to vertex shader's vPosition member
    mPositionHandle = glGetAttribLocation(mProgram, "vPosition");

	// get handle to fragment shader's TexCoord member
    mTexCoordHandle = glGetAttribLocation(mProgram, "TexCoordIn");
        
    // get handle to vertex shader's 
	mSizeHandle = glGetUniformLocation(mProgram, "Size");

	// get handle to shape's transformation matrix
    mMVPMatrixHandle = glGetUniformLocation(mProgram, "uMVPMatrix");

	// load into GPU
    // generate buffers
    GLuint *glbuffer = new GLuint[2];
    glGenBuffers(2, glbuffer);
    mVertexBuffer = glbuffer[0];
    mTexCoordBuffer = glbuffer[1];
	delete[] glbuffer;
        
    // upload vertex list
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 3 * 3 * 4, squareCoords, GL_STATIC_DRAW);  
    glBindBuffer(GL_ARRAY_BUFFER, 0);
        
    // upload tex coords
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 2 * 3 * 4, squareTexCoords, GL_STATIC_DRAW);  
    glBindBuffer(GL_ARRAY_BUFFER, 0);
        
	int  err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " after uploading particlemgr vertex buffer" << std::endl;
    }
}

particlemgr::~particlemgr()
{
}

void particlemgr::emit(int num, Texture *tex, partcb fnc, partinit initfnc, const vec *pos, const vec *vel)
{
	for (int i = 0; i < num; i++)
	{
		particle_data p;
		p.tex = tex;
		p.pcb = fnc;
		p.vel.set(vel);
		p.pos.set(pos);
		initfnc(&p);
		particles.push_back(p);
	}
}

bool runot(particle_data p) {return p.lifespan <= 0.f;}

void particlemgr::draw(double time_frame, const glm::mat4 &mVPMatrix, const glm::vec3 &campos)
{
	float time_frame_float = (float)time_frame;

	// remove particles that are run out
	particles.remove_if(runot);

	// callbacks
	for (auto p = particles.begin(); p != particles.end(); p++)
	{
		// move
		p->pos.x += p->vel.x * time_frame_float;
		p->pos.y += p->vel.y * time_frame_float;
		p->pos.z += p->vel.z * time_frame_float;
		p->pos.z -= p->gravity * time_frame_float;

		p->lifespan -= time_frame_float/16.f;

		p->pcb(&(*p), time_frame_float);
	}


	//////////////////////////////////////////////////////////////////////
	// drawing
	// Disable Depth Testing and activate other polygon order
	//glDisable(GL_DEPTH_TEST);  
	glFrontFace(GL_CCW);
		
	// Add program to OpenGL ES environment
	glUseProgram(mProgram);

	// Upload the projection and view transformation
	glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, glm::value_ptr(mVPMatrix));

	// get vectices into gpu
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glEnableVertexAttribArray(mPositionHandle);
    glVertexAttribPointer(mPositionHandle, 3,
                                    GL_FLOAT, false,
                                    12, 0);

	// get texture mapping into gpu
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuffer);
    glEnableVertexAttribArray(mTexCoordHandle);
    glVertexAttribPointer(mTexCoordHandle, 2, 
        						GL_FLOAT, false, 
        						8, 0);

	int  err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " after enabling particlemgr vertex buffer" << std::endl;
    }

	// TODO improvement: sort by texture, draw same textures together
	// or go back to resource-loader used in survive but cut out here
	
	// iterate through all particles
	for (particle_data p : particles)
	{
		// bind texture of this element
		glBindTexture(GL_TEXTURE_2D, p.tex->mTextureID);

		// set Matrix to identity
		glm::mat4 mTransformationMatrix = glm::mat4();
	        
		// move
		glm::vec3 t;
		t[0] = p.pos.x; t[1] = p.pos.y; t[2] = p.pos.z;
		mTransformationMatrix = glm::translate(mTransformationMatrix, t);
	        
		//turn
		vec angle;
		vec v (campos[0] - p.pos.x, campos[1] - p.pos.y, campos[2] - p.pos.z);
		angle.x = vec::angle(90.0f - vec::angle(atan2(v.x, v.y))*(float)(180.0/M_PI));
		angle.y = 90.f -vec::angle(asin(v.z / v.length())*(float)(180.0/M_PI));
		angle.z = 0.f;

		mTransformationMatrix = glm::rotate(mTransformationMatrix, glm::radians(angle.x), glm::vec3(0.f, 0.f, 1.f));
		mTransformationMatrix = glm::rotate(mTransformationMatrix, glm::radians(angle.y), glm::vec3(0.f, 1.f, 0.f));
		mTransformationMatrix = glm::rotate(mTransformationMatrix, glm::radians(angle.z), glm::vec3(1.f, 0.f, 0.f));
	        
		//scale
		glm::vec3 s;
		s[0] = p.size; s[1] = p.size, s[2] = p.size;
		mTransformationMatrix = glm::scale(mTransformationMatrix, s);
	        
	
		// Multiply with translation
		glm::mat4 mFinalMatrix = mVPMatrix * mTransformationMatrix;
	
		// Apply the projection and view transformation
		glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, glm::value_ptr(mFinalMatrix));	
		        	
		// Upload element alpha to fragment shader
		glUniform1f(mAlphaHandle, p.alpha);
		        	
		err = glGetError();
		if (err != 0) {
			std::cout << "OGL error code: " << err << " before draw" << std::endl;
		}

		// Draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 6);

		err = glGetError();
		if (err != 0) {
			std::cout << "OGL error code: " << err << " after draw" << std::endl;
		}
    }

	glDisableVertexAttribArray(mPositionHandle);
	glDisableVertexAttribArray(mTexCoordHandle);

	// Reset face value
	glFrontFace(GL_CW);

	err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " on particle drawing afterwards" << std::endl;
	}  
}




int particlemgr::loadShader(int type, const char *shaderCode)
{
    // create a vertex shader type (GL_VERTEX_SHADER)
    // or a fragment shader type (GL_FRAGMENT_SHADER)
    int shader = glCreateShader(type);

    // add the source code to the shader and compile it
	int len = strlen(shaderCode);
    glShaderSource(shader, 1, &shaderCode, &len);
    glCompileShader(shader);
        
    // Get the compilation status.
    int compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
     
    // If the compilation failed, delete the shader.
    if (compileStatus == 0)
    {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        std::cout << "particlemgr Shader compile error: " << strInfoLog << std::endl;
		delete[] strInfoLog;
		
		exit(-3);
    }

    return shader;
}
