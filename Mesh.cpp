#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SDL2/SDL.h"
#include "Sound.h"

// base header
typedef struct {
  int8_t version[4]; // "MDL3", "MDL4", or "MDL5"
  int32_t unused1;    // not used
  float scale[3];  // 3D position scale factors.
  float offset[3]; // 3D position offset.
  int32_t unused2;    // not used
  float unused3[3];// not used
  int32_t numskins;   // number of skin textures
  int32_t skinwidth;  // width of skin texture, for MDL3 and MDL4;
  int32_t skinheight; // height of skin texture, for MDL3 and MDL4;
  int32_t numverts;   // number of 3d wireframe vertices
  int32_t numtris;    // number of triangles surfaces
  int32_t numframes;  // number of frames
  int32_t numskinverts; // number of 2D skin vertices
  int32_t flags;     // always 0
  int32_t unused4;   // not used
} mdl_header;

// skin header
typedef struct {
  int32_t skintype; // 2 for 565 RGB, 3 for 4444 ARGB, 10 for 565 mipmapped, 11 for 4444 mipmapped (bpp = 2),
                 // 12 for 888 RGB mipmapped (bpp = 3), 13 for 8888 ARGB mipmapped (bpp = 4)
  int32_t width,height; // size of the texture
  /*byte skin[bpp*width*height]; // the texture image
  byte skin1[bpp*width/2*height/2]; // the 1st mipmap (if any)
  byte skin2[bpp*width/4*height/4]; // the 2nd mipmap (if any)
  byte skin3[bpp*width/8*height/8]; // the 3rd mipmap (if any)*/
} mdl5_skin_t;

// tex coords
typedef struct
{
  int16_t u; // position, horizontally in range 0..skinwidth-1
  int16_t v; // position, vertically in range 0..skinheight-1
} mdl_uvvert_t;

// triangles
typedef struct {
  int16_t index_xyz[3]; // Index of 3 3D vertices in range 0..numverts
  int16_t index_uv[3]; // Index of 3 skin vertices in range 0..numskinverts
} mdl_triangle_t;

#define CPYTRIS(A,B) A.index_xyz[0] = B.index_xyz[0]; A.index_xyz[1] = B.index_xyz[1]; A.index_xyz[2] = B.index_xyz[2]; A.index_uv[0] = B.index_uv[0]; A.index_uv[1] = B.index_uv[1]; A.index_uv[2] = B.index_uv[2]; 

typedef struct {
	uint16_t rawposition[3]; // X,Y,Z coordinate, packed on 0..65536
	uint8_t lightnormalindex; // index of the vertex normal
	uint8_t unused;
} mdl_trivertxs_t;

typedef struct {
	int32_t type; // 0 for byte-packed positions, and 2 for word-packed positions
	mdl_trivertxs_t bboxmin,bboxmax; // bounding box of the frame
	int8_t name[16]; // name of frame, used for animation
} mdl_frame_t;


Mesh::Mesh()
{
	loaded = false;
}


Mesh::Mesh(const char *mesh_file, const char *tex_file)
{
	loaded = false;

	// local storage vars
	mdl_header header;
	mdl5_skin_t *skins;
	mdl_uvvert_t *skinverts;
	mdl_triangle_t *tris;

	filename.assign(mesh_file);

	
	// open file
	//__android_log_print(ANDROID_LOG_VERBOSE, "libsdl", "opening file: %s", mesh_file);
	SDL_RWops *file_full = SDL_RWFromFile(mesh_file, "rb");
	if (file_full == NULL) {std::cout << "ERROR on SDL_RWFromFile while opening file: " << mesh_file << std::endl;  return;}
	
	int size = (int)file_full->size(file_full);
	uint8_t *buf = new uint8_t[size];
	SDL_RWread(file_full, buf, size, 1);
	SDL_RWclose(file_full);
	
	// buffer everything
	SDL_RWops *file = SDL_RWFromConstMem(buf, size);


	// read header
	SDL_RWread(file, &header, sizeof(mdl_header), 1);

	if (header.version[3] != '5') return; // only handle mdl5 for now

	numskins = header.numskins;
	numverts = header.numverts;
	numtris = header.numtris;
	numframes = header.numframes;
	
	std::cout << "Opened "<< mesh_file << " with " << numskins << " skins, " << numverts << " vertices, " << numtris << " triangles, " << numframes<< " frames." << std::endl;

	if (tex_file)
	{
	    //////////////////////////////////////////////////
	    // TODO: load texture from external file

		//glCompressedTexImage2D(GL_TEXTURE_2D, 0, ETC1.ETC1_RGB8_OES, tex.getWidth(), tex.getHeight(), 0, tex.getData().limit(), tex.getData());  

		// TODO: skip internally stored skins
    }
	else
	{
		skins = new mdl5_skin_t[numskins];

		mTextureID = new GLuint[numskins];
		glGenTextures(numskins, mTextureID);

		// read skins
		for (int i = 0; i < numskins; i++)
		{
			// bind current texture
			glBindTexture(GL_TEXTURE_2D, mTextureID[i]);
        
			// When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			//glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			// When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			
			#ifndef ANDROID
			glTexParameteri(GL_TEXTURE_2D,GL_GENERATE_MIPMAP,GL_TRUE);  // not in 3.x
			#endif

			// read skin header
			SDL_RWread(file,&(skins[i]), sizeof(mdl5_skin_t), 1);

			int hpp = 0;
			if (skins[i].skintype == 1) return; // 8 bit texture, not supported
			if (skins[i].skintype == 2 || skins[i].skintype == 3) hpp = 2;
			if (skins[i].skintype == 4) hpp = 3;
			if (skins[i].skintype == 5) hpp = 4;
			if (skins[i].skintype > 10) return; // mimapping not yet supported

			int imgsize = skins[i].width*skins[i].height*hpp;
			if (header.skinheight == 0) header.skinheight = skins[i].height;
			if (header.skinwidth == 0) header.skinwidth = skins[i].width;

			unsigned char *texture = new unsigned char[imgsize];
			SDL_RWread(file, texture, imgsize, 1);

			if (skins[i].skintype == 2) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, skins[i].width, skins[i].height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, texture);
        	if (skins[i].skintype == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skins[i].width, skins[i].height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, texture);
        	
        	if (skins[i].skintype == 4)
			{
				// from BGR to RGB
				for (int imageIdx = 0; imageIdx < imgsize; imageIdx += 3)
				{
					unsigned char colorSwap = texture[imageIdx];
					texture[imageIdx] = texture[imageIdx + 2];
					texture[imageIdx + 2] = colorSwap;
				}
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, skins[i].width, skins[i].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
			}
        	if (skins[i].skintype == 5)
			{
				for (int imageIdx = 0; imageIdx < imgsize; imageIdx += 4)
				{
					unsigned char colorSwap = texture[imageIdx];
					texture[imageIdx] = texture[imageIdx + 2];
					texture[imageIdx + 2] = colorSwap;
				}
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skins[i].width, skins[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
			}
        	
        	//if (skins[i].skintype == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, skins[i].width, skins[i].height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture);
        	//if (skins[i].skintype == 5) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skins[i].width, skins[i].height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture);
        	
			int err = glGetError();
			if (err != 0) {
				std::cout << "OGL error code: " << err << " on uploading skin " << i << ", skintype " << skins[i].skintype<< std::endl;
			}

			#ifdef ANDROID
        	glGenerateMipmap(GL_TEXTURE_2D); // not in opengl 2.1
        	#endif

			delete[] texture;
		}

		delete[] skins;
	}

	// load skin vertices
	skinverts = new mdl_uvvert_t[header.numskinverts];
	SDL_RWread(file, skinverts, sizeof(mdl_uvvert_t), header.numskinverts);

	// read triangles
	tris = new mdl_triangle_t[numtris];
	SDL_RWread(file, tris, sizeof(mdl_triangle_t), numtris);

	// transform texture coordinates
	float *texCoordBuffer = new float[numtris*3*2];
	int index;
	for (int i = 0; i < numtris; i++)
	{
		index = tris[i].index_uv[0];
		texCoordBuffer[i*3*2 + 0*2 + 0] = ((float)skinverts[index].u) / ((float)header.skinwidth);
		texCoordBuffer[i*3*2 + 0*2 + 1] = ((float)skinverts[index].v) / ((float)header.skinheight);

		index = tris[i].index_uv[1];
		texCoordBuffer[i*3*2 + 1*2 + 0] = ((float)skinverts[index].u) / ((float)header.skinwidth);
		texCoordBuffer[i*3*2 + 1*2 + 1] = ((float)skinverts[index].v) / ((float)header.skinheight);

		index = tris[i].index_uv[2];
		texCoordBuffer[i*3*2 + 2*2 + 0] = ((float)skinverts[index].u) / ((float)header.skinwidth);
		texCoordBuffer[i*3*2 + 2*2 + 1] = ((float)skinverts[index].v) / ((float)header.skinheight);
	}

    glGenBuffers(1, &mTexCoordBuffer);
	// upload texture corrdinates
	glBindBuffer(GL_ARRAY_BUFFER, mTexCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, numtris * 2 * 3 * 4, texCoordBuffer, GL_STATIC_DRAW);  
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] skinverts;
	delete[] texCoordBuffer;

	///////////////////////////////////////////////////////
	// read frames

	// vertex buffer for upload
	float *vertexBuffer = new float[numtris*3*3];
	// vertex buffer for inbetween storage
	float *vertexPosBuffer = new float[numverts*3];

	// generate buffers
    mVertexBuffer = new GLuint[numframes];
    glGenBuffers(numframes, mVertexBuffer);

	for (int f = 0; f < numframes; f++)
	{
		std::string s;
		mdl_frame_t frameheader;
		mdl_trivertxs_t trivert;
		SDL_RWread(file, &frameheader, sizeof(mdl_frame_t), 1);
		if (frameheader.type == 0) return; // not supporting byte packed frames!

		if (f == 1)
		{
			// store bb_max and min
			bb_max.x = (header.scale[0] * (float)frameheader.bboxmax.rawposition[0]) + header.offset[0];
			bb_max.y = (header.scale[1] * (float)frameheader.bboxmax.rawposition[1]) + header.offset[1];
			bb_max.z = (header.scale[2] * (float)frameheader.bboxmax.rawposition[2]) + header.offset[2];
			bb_min.x = (header.scale[0] * (float)frameheader.bboxmin.rawposition[0]) + header.offset[0];
			bb_min.y = (header.scale[1] * (float)frameheader.bboxmin.rawposition[1]) + header.offset[1];
			bb_min.z = (header.scale[2] * (float)frameheader.bboxmin.rawposition[2]) + header.offset[2];
			/*printf("bb_max.x = %f; bb_max.y = %f; bb_max.z = %f; \n", bb_max.x, bb_max.y, bb_max.z);
			printf("bb_min.x = %f; bb_min.y = %f; bb_min.z = %f; \n", bb_min.x, bb_min.y, bb_min.z);*/
		}
		frameheader.name[15] = 0; // make sure its null terminated!
		s.assign((char*)frameheader.name);
		frameNames.push_back(s);

		for (int i = 0; i < numverts; i++)
		{
			// read one entry
			SDL_RWread(file, &trivert, sizeof(mdl_trivertxs_t), 1);
			
			// safe into vertexPosBuffer
			// x, y, z
			vertexPosBuffer[i*3 + 0] = (header.scale[0] * (float)trivert.rawposition[0]) + header.offset[0];
			vertexPosBuffer[i*3 + 1] = (header.scale[1] * (float)trivert.rawposition[1]) + header.offset[1];
			vertexPosBuffer[i*3 + 2] = (header.scale[2] * (float)trivert.rawposition[2]) + header.offset[2];
		}

		// transform gathered information to a vertex list, for uploading
		for (int i = 0; i < numtris; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int ind = tris[i].index_xyz[j];
				vertexBuffer[i*3*3 + j*3 + 0] = vertexPosBuffer[3*ind + 0];
				vertexBuffer[i*3*3 + j*3 + 1] = vertexPosBuffer[3*ind + 1];
				vertexBuffer[i*3*3 + j*3 + 2] = vertexPosBuffer[3*ind + 2];
			}
		}

		// upload vertex list for one frame
	    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[f]);
	    glBufferData(GL_ARRAY_BUFFER, numtris * 3 * 3 * 4, vertexBuffer, GL_STATIC_DRAW);  
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	SDL_RWclose(file);

	delete[] tris;

	delete[] vertexBuffer;
	delete[] vertexPosBuffer;
	
	delete[] buf;

	loaded = true;

}

void Mesh::draw(const glm::mat4 &mVPMatrix)
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
        
        
    int err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " on drawing after disabling" << std::endl;
	}  
        
}

void Mesh::setShader() {
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
			"  gl_FragColor = texture2D(Texture, TexCoordOut) * vec4(1.10, 1.10, 0.8, 1.0) + vec4(coloring, 0.0); \n" 
			"  gl_FragColor.w *= alpha; \n"
            "}";
}

void Mesh::initShader() {
	
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
        
    // get handle to vertex shader's vNextPosition member
    mNextPositionHandle = glGetAttribLocation(mProgram, "vNextPosition");
        
    // get handle to vertex shader's other members
    mAnimProgressHandle = glGetUniformLocation(mProgram, "animProgress");
	mAlphaHandle = glGetUniformLocation(mProgram, "alpha");
	mColoringHandle = glGetUniformLocation(mProgram, "coloring");
        
    // get handle to shape's transformation matrix
    mMVPMatrixHandle = glGetUniformLocation(mProgram, "uMVPMatrix");
        
    int err = glGetError();
    if (err != 0) {
        std::cout << "OGL error code: " << err << " on shader initialization" << std::endl;
    }
}
    
    
int Mesh::loadShader(int type, const char *shaderCode) {
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
		std::cout << "Shader compile error: " << std::endl << strInfoLog << std::endl;
		delete[] strInfoLog;
		
		exit(-3);
    }

    return shader;
}
    
void Mesh::addRenderObject(RenderObject *obj) {
    objectList.push_back(obj);
}
    
void Mesh::removeRenderObject(RenderObject *obj) {

	// remove attached sounds
	for (int& i : obj->attachedSndChannels)
	{
		Mix_HaltChannel(i);
		Sound::channels3D_free[i] = true;
	}
	obj->attachedSndChannels.clear();
	

    return objectList.remove(obj);
}


int Mesh::animate(RenderObject* ro, const char* frame, float progress, int flags)
{
	// count number of frames with that name
	// TODO: make this static!
	int start_frame = -1;
	int num_frameset = 0;
	int ct = 0;
	for (std::string s : frameNames)
	{
		if (s.find(frame) == 0)
		{
			if (start_frame < 0) start_frame = ct;
			num_frameset++;
		}
		ct++;
	}
	if (num_frameset == 0) return -1;

	if (flags == 0)
	{
		if (progress > 100.f) progress = 100.f;
		num_frameset--; // Don't loop to start animation
	}
	else
	{
		// continuous animatino
		while (progress > 100.f) progress -= 100.f;
	}

	int target_frame = start_frame + (int)((progress/100.f) * num_frameset);
	if (target_frame >= numframes) {std::cout << "ERROR: frame too big on animation! How could this happen" << std::endl; return -1;}
	int target_next_frame = target_frame+1;
	if (target_next_frame >= start_frame + num_frameset) {if (flags != 0) target_next_frame = start_frame; else target_next_frame = target_frame;}
	if (target_next_frame >= numframes) target_next_frame = numframes-1;

	double intpart;
	ro->animProgress = (float)modf((progress/100.f) * num_frameset, &intpart);

	ro->animFrame = target_frame;
	ro->animNextFrame = target_next_frame;

	return target_frame;
}
