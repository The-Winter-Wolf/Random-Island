#ifndef _FBO_HG_
#define _FBO_HG_

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class cFBO
{
public:
	cFBO() : 
		ID(0), 
		colourTexture_0_ID(0),
		depthTexture_ID(0), 
		width(-1), height(-1),
		bHasColorBuffer(false),
		bHasDepthBuffer(false) {};

	GLuint ID;					// = 0;
	GLuint colourTexture_0_ID;	// = 0;
//	GLuint colourTexture_1_ID;	// = 0;
//	GLuint colourTexture_2_ID;	// = 0;
//	GLuint colourTexture_3_ID;	// = 0;

	GLuint depthTexture_ID;		// = 0;
	GLuint stencilTexture_ID;	// = 0;

	GLint width;				// in pixels;
	GLint height;				// in pixels;

	bool InitFBO(int width, int height, bool colorBuffer, bool depthBuffer, std::string &error);
	bool Shutdown(void);
	// Calls shutdown(), then init()
	bool Reset(int width, int height, std::string &error);
	
	void ClearBuffers(bool bClearColour = true, bool bClearDepth = true, bool bClearStencil = true);

	void ClearColourBuffer(int bufferindex);
	void ClearStencilBuffer( int clearColour, int mask = 0xFF );

	int GetMaxColourAttachments(void);
	int GetMaxDrawBuffers(void);

private:
	bool bHasColorBuffer;
	bool bHasDepthBuffer;
};

#endif
