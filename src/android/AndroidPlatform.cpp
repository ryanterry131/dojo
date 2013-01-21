#include "android/AndroidPlatform.h"

#include <jni.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android/native_activity.h>
#include <android_native_app_glue.h>

#include "Render.h"
#include "Game.h"
#include "Utils.h"
#include "Table.h"
#include "AndroidGLExtern.h"

/* android debug */
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "DOJO", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "DOJO", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DOJO", __VA_ARGS__))

using namespace Dojo;

/* ANDROID */
//status app
extern "C" int ANDROID_VALID_DEVICE;
//get app
extern  "C" struct android_app* GetAndroidApp();
extern  "C" const char* GetAndroidApk();
//EVENT APP
extern "C"  int32_t android_handle_input(struct android_app* app, AInputEvent* event) {
		//get platform
		AndroidPlatform* self = app->userData ? (AndroidPlatform*)app->userData : NULL;
		DEBUG_MESSAGE("android_handle_input");

		unsigned int flags = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
		if (flags == AMOTION_EVENT_ACTION_DOWN || flags == AMOTION_EVENT_ACTION_POINTER_DOWN) { //down

			int count = AMotionEvent_getPointerCount(event);
			for(int i = 0;i < count ; i++) 
					self->input->_fireTouchBeginEvent(Dojo::Vector(AMotionEvent_getX(event, i),
							                               AMotionEvent_getY(event, i)));

		} else if (flags == AMOTION_EVENT_ACTION_UP || flags == AMOTION_EVENT_ACTION_POINTER_UP) { //up

			int count = AMotionEvent_getPointerCount(event);
			for(int i = 0;i < count ; i++) 
					self->input->_fireTouchEndEvent(Dojo::Vector(AMotionEvent_getX(event, i),
							                             AMotionEvent_getY(event, i)));

		} else if (flags == AMOTION_EVENT_ACTION_MOVE) {                                           //move

			int count = AMotionEvent_getPointerCount(event);
			Dojo::Vector tmp;
			if(count) tmp=Dojo::Vector(AMotionEvent_getX(event, 0),AMotionEvent_getY(event, 0));
			if(count==1) self->input->_fireTouchMoveEvent(tmp,tmp);			
			for(int i = 1;i < count ; i++){ 
					self->input->_fireTouchMoveEvent(Dojo::Vector(AMotionEvent_getX(event, i),
							                              AMotionEvent_getY(event, i)),tmp);
					tmp=Dojo::Vector(AMotionEvent_getX(event,1),AMotionEvent_getY(event,1));
			}

		} else if (flags == AMOTION_EVENT_ACTION_CANCEL) {                                         //????? DOJO event??
			//save_fingers_input(input,event, IF_CANCEL);
		} else {
			return 0;
		}

		return 0;
	}
extern "C" void android_handle_cmd(struct android_app* app, int32_t cmd) {
		//get platform
		AndroidPlatform* self = app->userData ? (AndroidPlatform*)app->userData : NULL;
        	
		if( cmd==APP_CMD_INIT_WINDOW ){
			ANDROID_VALID_DEVICE=1; //enable Window_Flip() (and FOR INPUT)
        		DEBUG_MESSAGE("APP_INIT_WINDOW");
			if(self){
				//Reinizialize display
				self->ResetDisplay();
				//is RESUME
				self->isInPause=false;
			}
		}
		else
		if(cmd==APP_CMD_RESUME){
        		DEBUG_MESSAGE("APP_CMD_RESUME");
			//is RESUME
			if(self) self->isInPause=false;
		}
		else
		if( cmd==APP_CMD_TERM_WINDOW ){
        		DEBUG_MESSAGE("APP_CMD_TERM_WINDOW");
			ANDROID_VALID_DEVICE=0; //disable Window_Flip() (and FOR INPUT)
			//delete
			if(self){
				//IN PAUSE
				self->isInPause=true;
				//AND IN STOP LOOP
				self->running=false;
			}
		}
		else
		if(cmd==APP_CMD_PAUSE  && ANDROID_VALID_DEVICE ){
        		DEBUG_MESSAGE("APP_CMD_PAUSE");
			ANDROID_VALID_DEVICE=0; //disable Window_Flip() (and FOR INPUT)
			if(self){				
				//is IN PAUSE
				self->isInPause=true;
			}
			//waiting
		}else
		if (cmd==APP_CMD_GAINED_FOCUS){
		    // When our app gains focus, we start monitoring the accelerometer.
		   if (self != NULL && self->accelerometerSensor != NULL) {
		        ASensorEventQueue_enableSensor(self->sensorEventQueue,self->accelerometerSensor);
		        // We'd like to get 60 events per second (in us).
		        ASensorEventQueue_setEventRate(self->sensorEventQueue,self->accelerometerSensor,(1000L/60)*1000);
		    }
		}
		if(cmd==APP_CMD_LOST_FOCUS){
		    // When our app loses focus, we stop monitoring the accelerometer.
		    // This is to avoid consuming battery while not being used.
		    if (self != NULL && self->accelerometerSensor != NULL) {
		        ASensorEventQueue_disableSensor(self->sensorEventQueue,self->accelerometerSensor);
		    }
		}
	}


/* DOJO */
AndroidPlatform::AndroidPlatform(const Table& table) :
Platform(table){
    app=NULL;
    sensorManager=NULL;
    accelerometerSensor=NULL;
    sensorEventQueue=NULL;    

    addZipFormat( ".apk" );
    this->apkdir=String(GetAndroidApk());
    Utils::makeCanonicalPath(this->apkdir);
    DEBUG_MESSAGE("getPackageCodePath:");

}

void AndroidPlatform::ResetDisplay(){
 //initialize OpenGL ES and EGL


#ifdef DEF_SET_OPENGL_ES2
    const EGLint attribs[] = {
    //      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES2_BIT, //openGL ES 2.0 
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
	    EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
    };      
#else
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    }; 
#endif
    //SET ANDROID WINDOW
    EGLint _w, _h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    //get display
    DEBUG_MESSAGE("get display");
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    DEBUG_ASSERT( display );
    eglInitialize(display, 0, 0);
    //set openGL configuration
    DEBUG_MESSAGE("set openGL configuration:eglChooseConfig");
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    DEBUG_MESSAGE("set openGL configuration:eglGetConfigAttrib");
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    DEBUG_MESSAGE("set openGL ANativeWindow_setBuffersGeometry");
//ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_FULLSCREEN, 0);<--usare questo todo
//AWINDOW_FLAG_SCALED
    ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);
    //create a surface, and openGL context	
    DEBUG_MESSAGE("create a surface, and openGL context");
    surface = eglCreateWindowSurface(display, config,app->window, NULL);
    DEBUG_ASSERT( surface );
#ifdef SET_OPENGL_ES2
    const EGLint attrib_list [] = {EGL_CONTEXT_CLIENT_VERSION, 2,  EGL_NONE}; //openGL ES 2.0 //2, EGL_NONE
    context = eglCreateContext(display, config, NULL, attrib_list);
#else
    const EGLint attrib_list [] = {EGL_CONTEXT_CLIENT_VERSION, 1,  EGL_NONE}; //openGL ES 1.0 //1, EGL_NONE
    context = eglCreateContext(display, config, NULL, attrib_list);
#endif
    DEBUG_ASSERT( context );
    //set corrunt openGL context
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGE("error eglMakeCurrent");
        return;
    }
    //get WIDTH,HEIGHT
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
    DEBUG_MESSAGE("screen : width:"<<width<<" height:"<<height);
    //init opengl VBA calls	
    DEBUG_MESSAGE("!!init opengl VBA calls!");
    ExternInitOpenGL();
    //is not in pause
    isInPause=false;
    //
}
void AndroidPlatform::initialise(Game *g)
{
    DEBUG_MESSAGE("AndroidPlatform::initialise()");
    //set game
    game=g;
    DEBUG_ASSERT( game );
    //init app
    DEBUG_ASSERT( GetAndroidApp() );
    GetAndroidApp()->userData=(void*)this;
    this->app=GetAndroidApp();
    //Set Display	
    ResetDisplay();
    //accelerometer
    this->sensorManager = ASensorManager_getInstance();
    DEBUG_ASSERT( sensorManager );

    this->accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,ASENSOR_TYPE_ACCELEROMETER);
    DEBUG_ASSERT( accelerometerSensor );

    //dojo object
    render = new Render( ((int)width), ((int)height), DO_LANDSCAPE_LEFT );
    input  = new InputSystem();
    sound  = new SoundManager();
    //enable loop
    running=true;
    //start the game
    game->begin();
}   

String AndroidPlatform::getAppDataPath(){ 
	return this->apkdir+"/assets";
 }
String AndroidPlatform::getResourcesPath(){ 
 	std::string tmp=(this->apkdir+String("/assets")).ASCII();
	DEBUG_MESSAGE("AndroidPlatform::getResourcesPath:"<<tmp);
	return this->apkdir+String("/assets");
 }
String AndroidPlatform::getRootPath(){ 
 	std::string tmp=this->apkdir.ASCII();
	DEBUG_MESSAGE("AndroidPlatform::getRootPath:"<<tmp);
	return this->apkdir; 
}

void AndroidPlatform::shutdown()
{
        DEBUG_MESSAGE("AndroidPlatform::shutdown()");
	// and a cheesy fade exit
	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (context != EGL_NO_CONTEXT) {
		    eglDestroyContext(display, context);
		}
		if (surface != EGL_NO_SURFACE) {
		    eglDestroySurface(display, surface);
		}
		eglTerminate(display);
	}
    	display = EGL_NO_DISPLAY;
    	context = EGL_NO_CONTEXT;
    	surface = EGL_NO_SURFACE;
	//enable loop, and disable draw (pause)
	running=false;
	isInPause=true;
}       

void AndroidPlatform::acquireContext()
{
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        DEBUG_MESSAGE("AndroidPlatform::acquireContext() error eglMakeCurrent:");
        DEBUG_MESSAGE("AndroidPlatform::display"<<display);
        DEBUG_MESSAGE("AndroidPlatform::surface"<<surface);
        DEBUG_MESSAGE("AndroidPlatform::context"<<context);
        return;
    }
}

GLenum AndroidPlatform::loadImageFile( void*& bufptr, const String& path, int& width, int& height, int& pixelSize )
{
	void* data;

        DEBUG_MESSAGE("loadImageFile::path:"<<path.ASCII());

	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib = NULL;

	std::string ansipath = path.ASCII();

	//I know that FreeImage can deduce the fif from file, but I want to enforce correct filenames
	fif = FreeImage_GetFIFFromFilename(ansipath.c_str());
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return 0;

	//check that the plugin has reading capabilities and load the file
	if( !FreeImage_FIFSupportsReading(fif))
		return 0;

	char* buf;
	int fileSize = loadFileContent( buf, path );

	// attach the binary data to a memory stream
	FIMEMORY *hmem = FreeImage_OpenMemory( (BYTE*)buf, fileSize );

	// load an image from the memory stream
	dib = FreeImage_LoadFromMemory(fif, hmem, 0);

	//if the image failed to load, return failure
	if(!dib)
		return 0;

	//retrieve the image data
	data = (void*)FreeImage_GetBits(dib);
	//get the image width and height, and size per pixel
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	
	pixelSize = FreeImage_GetBPP(dib)/8;
	
	int size = width*height*pixelSize;
	bufptr = malloc( size );
	
	//swap R and B and invert image while copying
	byte* in, *out;
	for( int i = 0, ii = height-1; i < height ; ++i, --ii )
	{
		for( int j = 0; j < width; ++j )
		{
			out = (byte*)bufptr + (j + i*width)*pixelSize;
			in = (byte*)data + (j + ii*width)*pixelSize;

			if( pixelSize >= 4 )
				out[3] = in[3];
			
			if( pixelSize >= 3 )
			{
				out[2] = in[0];
				out[1] = in[1];
				out[0] = in[2];
			}
			else
			{
				out[0] = in[0];
			}
		}
	}
	
	//free resources
	FreeImage_Unload( dib );
	FreeImage_CloseMemory(hmem);
	free( buf );

        DEBUG_MESSAGE("loadImageFile::size:"<<width<<"x"<<height);

	static const GLenum formatsForSize[] = { GL_NONE, GL_UNSIGNED_BYTE, 0, GL_RGB, GL_RGBA };
	return formatsForSize[ pixelSize ];
}

void AndroidPlatform::present()
{
   // No display or in pause....
   if ( display == EGL_NO_DISPLAY || isInPause ){
        	DEBUG_MESSAGE("no AndroidPlatform::present()");
		return;
   } 
   eglSwapBuffers( display, surface);
}

void AndroidPlatform::step( float dt )
{
	DEBUG_ASSERT( running );
	//update accelerometer	
	UpdateEvent();
	//update game
	game->loop( dt );
	render->render();	
	//sound->update( dt );

}

void AndroidPlatform::UpdateEvent(){


	if(sensorEventQueue==NULL){

 		DEBUG_MESSAGE("UpdateEvent::init createEventQueue");

		sensorEventQueue =ASensorManager_createEventQueue(sensorManager,
								  app->looper,
								  LOOPER_ID_USER,
								  NULL, NULL);

 		DEBUG_MESSAGE("UpdateEvent::init enableSensor");

		 ASensorEventQueue_enableSensor(sensorEventQueue,accelerometerSensor);
		// We'd like to get 60 events per second (in us).

 		DEBUG_MESSAGE("UpdateEvent::init setEventRate");

		ASensorEventQueue_setEventRate(sensorEventQueue,
		                               accelerometerSensor,
		                               (1000L/60)*1000);
	}

	int ident,events;
	struct android_poll_source* source;

	while ((ident = ALooper_pollAll(0, NULL, &events,(void**)&source)) >= 0){
		if (source != NULL) source->process(app, source);
		//GET SENSOR
                if (ident == LOOPER_ID_USER) {
                    if (accelerometerSensor != NULL) {
                        ASensorEvent event;
                        while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
			   input->_fireAccelerationEvent(Dojo::Vector(event.acceleration.x,
								      event.acceleration.y,
								      event.acceleration.z),0);
                        }
                    }
                }
		if (app->destroyRequested != 0) {
			return;
		}
	}

}

void AndroidPlatform::loop()
{
	DEBUG_ASSERT( game );
	float frameInterval = game->getNativeFrameLength();

	frameTimer.reset();
	float dt;

    	DEBUG_MESSAGE("start Platform::loop()");

	while( running )
	{
		dt = frameTimer.getElapsedTime();
		if( dt > frameInterval )
		{
			frameTimer.reset();
			if(!isInPause) step( dt );
		}
	}
	shutdown();
}