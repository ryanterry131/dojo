#include "stdafx.h"

#include "ShaderProgram.h"

#include "Platform.h"
#include "FileStream.h"

using namespace Dojo;

///"real file" Resource constructor. When onLoad is called, it will use filePath to load its contents
ShaderProgram::ShaderProgram( ResourceGroup* creator, const String& filePath ) : 
	Resource( creator, filePath )
{
	//guess the type from the extension
	String ext = Utils::getFileExtension( filePath );

	if( ext == L"vs" )	mType = SPT_VERTEX;
	else if( ext == L"ps" )	mType = SPT_FRAGMENT;
	else
		DEBUG_FAIL( "Unsupported shader type" );
}

#ifdef DOJO_SHADERS_AVAILABLE

bool ShaderProgram::_load()
{
	static const GLuint typeGLTypeMap[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

	GLint compiled, sourceLength = mContentString.size();
	const char* src = mContentString.c_str();

	mGLShader = glCreateShader( typeGLTypeMap[mType] );

	glShaderSource( mGLShader, 1, &src, &sourceLength ); //load the program source

	glCompileShader( mGLShader );

	CHECK_GL_ERROR;

	glGetObjectParameterivARB( mGLShader, GL_COMPILE_STATUS, &compiled);
	loaded = compiled != 0;

	if (!loaded)
	{
		//grab some info about the error
		GLint blen = 0;	
		GLsizei slen = 0;

		glGetShaderiv( mGLShader, GL_INFO_LOG_LENGTH , &blen);
		if (blen > 1)
		{
			GLchar* compiler_log = (GLchar*)malloc(blen);
			glGetInfoLogARB( mGLShader, blen, &slen, compiler_log);
			DEBUG_MESSAGE( "Compiler error:\n" << compiler_log );
			free (compiler_log);
		}	

		DEBUG_FAIL("A shader program failed to compile!");
	}

	return loaded;
}

void ShaderProgram::onUnload( bool soft /* = false */ )
{
	DEBUG_ASSERT( isLoaded(), "This Shader Program is already unloaded" );

	glDeleteShader( mGLShader );
	mGLShader = GL_NONE;

	loaded = false;
}

#else

bool ShaderProgram::_load()
{
	DEBUG_FAIL( "Shaders not supported" );
	return false;
}

void ShaderProgram::onUnload( bool soft /* = false */ )
{
	DEBUG_FAIL( "Shaders not supported" );
}
#endif

bool ShaderProgram::onLoad()
{
	DEBUG_ASSERT( !isLoaded(), "Cannot reload an already loaded program" );

	if( getFilePath().size() ) //try loading from file
	{
		auto file = Platform::getSingleton()->getFile( filePath );

		if( file->open() ) //open the file
		{
			mContentString.clear();
			mContentString.resize( file->getSize() );

			file->read( (byte*)mContentString.c_str(), mContentString.size() );

			loaded = _load(); //load from the temp buffer
		}
	}
	else //load from the in-memory string
	{
		loaded = _load();
	}

	return loaded;
}