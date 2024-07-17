#include "cFBO.h"


// Calls Shutdown(), then Init()
bool cFBO::Reset(int width, int height, std::string &error)
{
	if ( ! this->Shutdown() )
	{
		error = "Could not shutdown";
		return false;
	}

	return this->InitFBO( width, height, bHasColorBuffer, bHasDepthBuffer, error );
}

bool cFBO::Shutdown(void)
{
	if (bHasColorBuffer)
	{
		glDeleteTextures( 1, &(this->colourTexture_0_ID) );
	}
	if (bHasDepthBuffer)
	{
		glDeleteTextures( 1, &(this->depthTexture_ID) );
	}

	glDeleteFramebuffers( 1, &(this->ID) );

	return true;
}


bool cFBO::InitFBO(int width, int height, bool colorBuffer, bool depthBuffer, std::string& error)
{
	this->width = width;
	this->height = height;
	this->bHasColorBuffer = colorBuffer;
	this->bHasDepthBuffer = depthBuffer;

//	glCreateFramebuffers(1, &( this->ID ) );	// GL 4.5
	glGenFramebuffers( 1, &( this->ID ) );		// GL 3.0
	glBindFramebuffer(GL_FRAMEBUFFER, this->ID);

//****************************************************************************************
 
	if (bHasColorBuffer)
	{
		// Create the colour buffer (texture)
		glGenTextures(1, &(this->colourTexture_0_ID ) );
		glBindTexture(GL_TEXTURE_2D, this->colourTexture_0_ID);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8 /*GL_RGBA32F*/, this->width, this->height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->colourTexture_0_ID, 0);

		static const GLenum draw_bufers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_bufers);	// There are 4 outputs now
	}

//**********************************************************************************************

	if (bHasDepthBuffer)
	{
		// Create the depth buffer (texture)
		glGenTextures(1, &( this->depthTexture_ID ));
		glBindTexture(GL_TEXTURE_2D, this->depthTexture_ID);

		// GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT16, 
		// GL_DEPTH32F_STENCIL8, GL_DEPTH24_STENCIL8 (more common), GL_STENCIL_INDEX8 (only stencil).
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, this->width, this->height);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, this->width, this->height);

		//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT );
		//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_COMPONENT );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_TEXTURE_2D, this->depthTexture_ID, 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthTexture_ID, 0);
		// or
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, this->depthTexture_ID, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthTexture_ID, 0);
	}

// *****************************************************************************************************

	if (bHasColorBuffer)
	{
		static const GLenum draw_bufers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_bufers);	// There are 4 outputs now
	}
	else
	{
		glDrawBuffer(GL_NONE); // There aren't color outputs
		glReadBuffer(GL_NONE); // There aren't color inputs
	}

// ****************************************************

	bool bFrameBufferIsGoodToGo = true; 

	switch ( glCheckFramebufferStatus(GL_FRAMEBUFFER) )
	{
	case GL_FRAMEBUFFER_COMPLETE:
		bFrameBufferIsGoodToGo = true; 
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		bFrameBufferIsGoodToGo = false; 
		break;
//	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
	case GL_FRAMEBUFFER_UNSUPPORTED:
	default:
		bFrameBufferIsGoodToGo = false; 
		break;
	}

	// Point back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return bFrameBufferIsGoodToGo;
}

void cFBO::ClearColourBuffer(int bufferindex)
{
	glViewport(0, 0, this->width, this->height);
	GLfloat	zero = 0.0f;
	glClearBufferfv(GL_COLOR, bufferindex, &zero);

	return;
}

void cFBO::ClearStencilBuffer(int clearColour, int mask)
{
	glStencilMask(mask);
	glClearBufferiv(GL_STENCIL, 0, &clearColour);
}


void cFBO::ClearBuffers(bool bClearColour, bool bClearDepth, bool bClearStencil)
{
	glViewport(0, 0, this->width, this->height);

	GLint intZero = 0;

	GLfloat one = 1.0f;
	GLfloat	zero = 0.0f;

	if ( bClearColour )
	{
		glClearBufferfv(GL_COLOR, 0, &zero); // Colour
	}
	if ( bClearDepth )
	{
		glClearBufferfv(GL_DEPTH, 0, &one);	 // Depth is normalized 0.0f to 1.0f
	}
	// If buffer is GL_STENCIL, drawbuffer must be zero, and value points to a 
	//  single value to clear the stencil buffer to. Masking is performed in the 
	//  same fashion as for glClearStencil. Only the *iv forms of these commands 
	//  should be used to clear stencil buffers; be used to clear stencil buffers; 
	//  other forms do not accept a buffer of GL_STENCIL.
	
	glStencilMask(0xFF);

	if ( bClearStencil )
	{
		glClearBufferiv(GL_STENCIL, 0, &intZero );	
	}

	if (bClearDepth && bClearStencil)
	{
		glClearBufferfi( GL_DEPTH_STENCIL, 0, 1.0f /*depth*/, 0 /*stencil*/ );
	}

	return;
}


int cFBO::GetMaxColourAttachments(void)
{
	int maxColourAttach = 0;
	glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &maxColourAttach );

	return maxColourAttach;
}

int cFBO::GetMaxDrawBuffers(void)
{
	int maxDrawBuffers = 0;
	glGetIntegerv( GL_MAX_DRAW_BUFFERS, &maxDrawBuffers );
	
	return maxDrawBuffers;
}
