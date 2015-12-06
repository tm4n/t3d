#include "MeshTerrain.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

MeshTerrain::MeshTerrain(const char *mesh_file, const char *tex_file) : Mesh(mesh_file, tex_file)
{
}

void MeshTerrain::setShader()
{
	// Set shader content
	vertexShaderCode =
            // This matrix member variable provides a hook to manipulate
            // the coordinates of the objects that use this vertex shade
			"#version 110 \n"
			"#ifdef GL_ES \n"
			"precision mediump float; \n"
			"#endif \n"
			
            "uniform mat4 uMVPMatrix; \n"

            "attribute vec4 vPosition; \n"
            "attribute vec2 TexCoordIn; \n" 
            "varying vec2 TexCoordOut; \n"
            "void main() { \n"
            // the matrix must be included as a modifier of gl_Position
            " gl_Position = uMVPMatrix * vPosition; \n"
            " TexCoordOut = TexCoordIn; \n" 
            "}";

    fragmentShaderCode =
			"#version 110 \n"
			"#ifdef GL_ES \n"
			"precision mediump float; \n"
			"#endif \n"
			
            "varying vec2 TexCoordOut; \n" 
            "uniform sampler2D Texture1; \n"
			"uniform sampler2D Texture2; \n"
            "void main() { \n" 
            //"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
			"  gl_FragColor = (texture2D(Texture1, TexCoordOut) + (vec4(0.5, 0.5, 0.5, 0.0) - texture2D(Texture2, TexCoordOut*100.0)*0.7)) * vec4(1.1, 1.1, 0.8, 1.0);\n" //vec4(0.22, 0.18, 0.0, 0.0); \n"
			"  gl_FragColor.w = 1.0; \n"
			//"  gl_FragColor = (texture2D(Texture1, TexCoordOut) + texture2D(Texture2, TexCoordOut*100.0) / 1.4) +  vec4(-0.05, -0.05, -0.25, 0.0); \n" //* vec4(1.0, 1.0, 0.7, 1.0) ;\n" 
            "}";
}


void MeshTerrain::draw(const glm::mat4 &mVPMatrix)
{
	if (objectList.empty()) return;

    // Add program to OpenGLenvironment
    glUseProgram(mProgram);


    // get texture mapping into gpu
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuffer);
    glEnableVertexAttribArray(mTexCoordHandle);
    glVertexAttribPointer(mTexCoordHandle, 2, 
        						GL_FLOAT, GL_FALSE, 
        						2*4, 0); 
        
	// Upload both textures!
    glActiveTexture(GL_TEXTURE0); //- activating texture unit 0
    glBindTexture(GL_TEXTURE_2D, mTextureID[0]);
	glUniform1i(mTexHandle, 0); //- setting texture unit 0

	glActiveTexture(GL_TEXTURE1); //- activating texture unit 1
    glBindTexture(GL_TEXTURE_2D, mTextureID[1]);
	glUniform1i(mTexHandle2, 1); //- setting texture unit 1

    for (RenderObject *obj : objectList) {
        	
        // get vertex positions into gpu
        // Bind vertex buffer object for triangle vertices
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[obj->animFrame]);
            
        // Enable a handle to the triangle vertices
        glEnableVertexAttribArray(mPositionHandle);

        // Prepare the triangle coordinate data
        glVertexAttribPointer(mPositionHandle, 3,
                                        GL_FLOAT, GL_FALSE,
                                        0, 0);

		int err = glGetError();
		if (err != 0) {
			std::cout << "OGL error code: " << err << " on drawing after Pos data" << std::endl;
		}     	
        	
	    // calculate translation and movement

		// set Matrix to identity
	    mTransformationMatrix = glm::mat4();
	        
	    // move
		mTransformationMatrix = glm::translate(mTransformationMatrix, obj->translation);
	        
	    //turn
		mTransformationMatrix = glm::rotate(mTransformationMatrix, glm::radians(obj->rotation[0]), glm::vec3(0.f, 0.f, 1.f));
		mTransformationMatrix = glm::rotate(mTransformationMatrix, glm::radians(obj->rotation[1]), glm::vec3(0.f, 1.f, 0.f));
		mTransformationMatrix = glm::rotate(mTransformationMatrix, glm::radians(obj->rotation[2]), glm::vec3(1.f, 0.f, 0.f));
	        
	    //scale
	    mTransformationMatrix = glm::scale(mTransformationMatrix, obj->scale);
	        
	
	    // Multiply with translation
	    mFinalMatrix = mVPMatrix * mTransformationMatrix;
	
	    // Apply the projection and view transformation
	    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, &mFinalMatrix[0][0]);

		err = glGetError();
		if (err != 0) {
			std::cout << "OGL error code: " << err << " on drawing after all uploads" << std::endl;
		}
	        
	    // Draw the triangles
	    glDrawArrays(GL_TRIANGLES, 0, numtris*3);

		err = glGetError();
		if (err != 0) {
			std::cout << "OGL error code: " << err << " on drawing after drawing" << std::endl;
		}

    }

    // Disable vertex array
    glDisableVertexAttribArray(mTexCoordHandle);
        
    glDisableVertexAttribArray(mPositionHandle);

        
    // unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        
    int err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " on drawing after disabling" << std::endl;
	}  
        
}


void MeshTerrain::initShader() {
	
	setShader();

    // initialize shaders
    int vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderCode);
    int fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    mProgram = glCreateProgram();             // create empty OpenGL ES Program
    glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
    glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
    glLinkProgram(mProgram);                  // creates OpenGL ES program executables
        
    // get handle to fragment shader's TexCoord member
    mTexCoordHandle = glGetAttribLocation(mProgram, "TexCoordIn");
        
    // get handle to vertex shader's vPosition member
    mPositionHandle = glGetAttribLocation(mProgram, "vPosition");
        
    // get handle to shape's transformation matrix
    mMVPMatrixHandle = glGetUniformLocation(mProgram, "uMVPMatrix");

	// get handle to texture samplers
	mTexHandle = glGetUniformLocation(mProgram, "Texture1");
	mTexHandle2 = glGetUniformLocation(mProgram, "Texture2");
        
    int err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " on drawing" << std::endl;
    }
}
