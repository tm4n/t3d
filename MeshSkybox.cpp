#include "MeshSkybox.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLfloat skyboxVertices[] = {
	// Positions          
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};

MeshSkybox::MeshSkybox(CubeTexture *cubeTex) : Mesh()
{
	this->tex = cubeTex;

	glGenBuffers(1, &mVertexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int err = glGetError();
	if (err != 0) {
		std::cout << "OGL error code: " << err << " on creating skybox after Pos data" << std::endl;
	}
}

void MeshSkybox::setShader()
{
	// Set shader content
	vertexShaderCode =
		// This matrix member variable provides a hook to manipulate
		// the coordinates of the objects that use this vertex shade
		"#version 110 \n"
		"#ifdef GL_ES \n"
		"precision mediump float; \n"
		"#endif \n"

		"uniform mat4 MVPMatrix; \n"

		"attribute vec3 vPosition; \n"
		"varying vec3 TexCoord0; \n"
		"void main() { \n"
		// the matrix must be included as a modifier of gl_Position
		" vec4 WVP_Pos = MVPMatrix * vec4(vPosition, 1.0); \n"
		" gl_Position = WVP_Pos.xyww; \n"
        " TexCoord0 = vPosition; \n" 
        "}";

	fragmentShaderCode =
			"#version 110 \n"
			"#ifdef GL_ES \n"
			"precision mediump float; \n"
			"#endif \n"

			"varying vec3 TexCoord0; \n"
			"uniform samplerCube Skybox; \n"

			"void main() { \n"

			"  gl_FragColor = textureCube(Skybox, TexCoord0); \n"

            "}";
}


void MeshSkybox::draw(const glm::mat4 &projection, const glm::mat4 &view)
{
	glFrontFace(GL_CCW); // set standard polygon mode
	glDepthMask(GL_FALSE);// Remember to turn depth writing off

    // Add program to OpenGLenvironment
    glUseProgram(mProgram);
        
	// Bind texture
    glActiveTexture(GL_TEXTURE0); //- activating texture unit 0
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex->mTextureID);
	glUniform1i(mTexHandle, 0); //- setting texture unit 0
        	
    // get vertex positions into gpu
    // Bind vertex buffer object for triangle vertices
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            
    // Enable a handle to the triangle vertices
    glEnableVertexAttribArray(mPositionHandle);

    // Prepare the triangle coordinate data
    glVertexAttribPointer(mPositionHandle, 3,
                                    GL_FLOAT, GL_FALSE,
                                    0, 0);

	int err = glGetError();
	if (err != 0) {
		std::cout << "OGL error code: " << err << " on drawing skybox after Pos data" << std::endl;
	}     	

	// Apply the projection and view transformation
	glm::mat4 viewnotrans = glm::mat4(glm::mat3(view));
	mTransformationMatrix = glm::mat4();
	mTransformationMatrix = glm::rotate(mTransformationMatrix, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	mFinalMatrix = (projection * viewnotrans) * mTransformationMatrix;

	glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, glm::value_ptr(mFinalMatrix));

	err = glGetError();
	if (err != 0) {
		std::cout << "OGL error code: " << err << " on drawing skybox after all uploads" << std::endl;
	}
	        
	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 36);

	err = glGetError();
	if (err != 0) {
		std::cout << "OGL error code: " << err << " on drawing skybox after drawing" << std::endl;
	}

    // Disable vertex array      
    glDisableVertexAttribArray(mPositionHandle);

        
    // unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	// reenable defaults
	glDepthMask(GL_TRUE);
	glFrontFace(GL_CW);

	err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " on drawing skybox after disabling" << std::endl;
	}
        
}


void MeshSkybox::initShader() {
	
	setShader();

    // initialize shaders
    int vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderCode);
    int fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    mProgram = glCreateProgram();             // create empty OpenGL ES Program
    glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
    glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
    glLinkProgram(mProgram);                  // creates OpenGL ES program executables
        
        
    // get handle to vertex shader's vPosition member
    mPositionHandle = glGetAttribLocation(mProgram, "vPosition");
        
    // get handle to other matrices
	mMVPMatrixHandle = glGetUniformLocation(mProgram, "MVPMatrix");

	// get handle to texture samplers
	mTexHandle = glGetUniformLocation(mProgram, "Skybox");
        
    int err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " on skybox shader init" << std::endl;
    }
}
