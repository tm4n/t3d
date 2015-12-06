#include "MeshBlend.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

MeshBlend::MeshBlend(const char *mesh_file, const char *tex_file) : Mesh(mesh_file, tex_file)
{
}

void MeshBlend::setShader() {
	// Set shader content
	vertexShaderCode =
            // This matrix member variable provides a hook to manipulate
            // the coordinates of the objects that use this vertex shade
			"#version 110 \n"
			"#ifdef GL_ES \n"
			"precision mediump float; \n"
			"#endif \n"
			
            "uniform mat4 uMVPMatrix; \n"
            "uniform float animProgress; \n"

            "attribute vec4 vPosition; \n"
            "attribute vec4 vNextPosition; \n"
            "attribute vec2 TexCoordIn; \n" 
            "varying vec2 TexCoordOut; \n"
            "void main() { \n"
            // the matrix must be included as a modifier of gl_Position
            " gl_Position = uMVPMatrix * mix(vPosition, vNextPosition, animProgress); \n"
            " TexCoordOut = TexCoordIn; \n" 
            "}";

    fragmentShaderCode =
			"#version 110 \n"
			"#ifdef GL_ES \n"
			"precision mediump float; \n"
			"#endif \n"
			
            "varying vec2 TexCoordOut; \n" 
            "uniform sampler2D Texture; \n"
			"uniform float alpha; \n"
			"uniform vec3 coloring; \n"
            "void main() { \n" 
            //"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);" +
			"  gl_FragColor = texture2D(Texture, TexCoordOut) * alpha + vec4(coloring, 0.0); \n" 
			//"  gl_FragColor.w *= alpha; \n"
            "}";
}


void MeshBlend::draw(const glm::mat4 &mVPMatrix)
{
	if (objectList.empty()) return;

    // Add program to OpenGLenvironment
    glUseProgram(mProgram);

	// enable blending
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE);


    // get texture mapping into gpu
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuffer);
    glEnableVertexAttribArray(mTexCoordHandle);
    glVertexAttribPointer(mTexCoordHandle, 2, 
        						GL_FLOAT, GL_FALSE, 
        						2*4, 0); 
        
    // int mtexUniform = glGetUniformLocation(mProgram, "Texture");
    glActiveTexture(GL_TEXTURE0); //- activating texture unit 0
    glBindTexture(GL_TEXTURE_2D, mTextureID[0]);
	//glUniform1i(mtexUniform, 0); //- setting texture unit 0

    for (RenderObject *obj : objectList) {
        
		if (obj->visible == false) continue;

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

		// Enable a handle to the triangle vertices
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[obj->animNextFrame]);
		glEnableVertexAttribArray(mNextPositionHandle);

		// Prepare the triangle coordinate data
		glVertexAttribPointer(mNextPositionHandle, 3,
										GL_FLOAT, GL_FALSE,
										0, 0);
            
		err = glGetError();
		if (err != 0) {
			std::cout << "OGL error code: " << err << " on drawing after NextPos data" << std::endl;
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
	    glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, glm::value_ptr(mFinalMatrix));
	        
	    // Upload other values
	    glUniform1f(mAnimProgressHandle, obj->animProgress);
		glUniform1f(mAlphaHandle, obj->alpha);
		glUniform3fv(mColoringHandle, 1, glm::value_ptr(obj->coloring));

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
        
    glDisableVertexAttribArray(mNextPositionHandle);
        
    // unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
        
	// disable blending:
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
    int err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " on drawing after disabling" << std::endl;
	}  
        
}
