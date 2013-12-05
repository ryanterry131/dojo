#include "stdafx.h"

#include "Render.h"

#include "Renderable.h"
#include "TextArea.h"
#include "Platform.h"
#include "Viewport.h"
#include "Light.h"
#include "Mesh.h"
#include "AnimatedQuad.h"
#include "Shader.h"

#include "Game.h"

using namespace Dojo;

Render::Render( uint w, uint h, Orientation deviceOr ) :
frameStarted( false ),
valid( true ),
width( w ),
height( h ),
renderOrientation( DO_LANDSCAPE_RIGHT ),
deviceOrientation( deviceOr ),
currentLayer( NULL ),
frameVertexCount(0),
frameTriCount(0),
frameBatchCount(0),
backLayer( NULL )
{
	platform = Platform::getSingleton();	

	DEBUG_MESSAGE( "Creating OpenGL context...");
	DEBUG_MESSAGE ("querying GL info... ");
	DEBUG_MESSAGE ("vendor: " + String( (const char*)glGetString (GL_VENDOR)));
	DEBUG_MESSAGE ("renderer: " + String( (const char*)glGetString (GL_RENDERER)));
	DEBUG_MESSAGE ("version: OpenGL " + String( (const char*)glGetString (GL_VERSION)));
	
	//clean errors (some drivers leave errors on the stack)
	CHECK_GL_ERROR;
	CHECK_GL_ERROR;
	
	glEnable( GL_RESCALE_NORMAL );
	glEnable( GL_NORMALIZE );
	glEnable( GL_CULL_FACE );

	glCullFace( GL_BACK );
	
	glShadeModel( GL_SMOOTH );

	//default status for blending
	glEnable( GL_BLEND );	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glEnable( GL_COLOR_MATERIAL );
	
	//on IOS this is default and the command is not supported
#ifndef USING_OPENGLES
	glColorMaterial( GL_FRONT, GL_DIFFUSE );
#endif	
	
#ifdef DOJO_GAMMA_CORRECTION_ENABLED
	glEnable( GL_FRAMEBUFFER_SRGB );
#endif
	
	//projection is always the same
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();		

	//always active!
	glEnableClientState(GL_VERTEX_ARRAY);

	currentRenderState = firstRenderState = new RenderState();

	///create the back layer
	backLayer = new Layer();
	backLayer->add( NULL ); //add a dummy element

	setInterfaceOrientation( platform->getGame()->getNativeOrientation() );
	
	setDefaultAmbient( Color::BLACK );

	CHECK_GL_ERROR;
}

Render::~Render()
{
	SAFE_DELETE( firstRenderState );
	SAFE_DELETE( backLayer );

	clearLayers();
}

void Render::setWireframe( bool wireframe )
{
#ifndef USING_OPENGLES
	glPolygonMode( GL_FRONT_AND_BACK, (wireframe) ? GL_LINE : GL_FILL );
#else
    DEBUG_FAIL( "Wireframe mode is not supported on OpenGLES!" );
#endif
}

Render::Layer* Render::getLayer( int layerID )
{	
	LayerList* layerList = &positiveLayers;
	if( layerID < 0 )
	{
		layerID = -layerID - 1; //flip and shift by 1
		layerList = &negativeLayers;
	}
	
	//allocate the needed layers if layerID > layer size
	while( layerList->size() <= layerID )
		layerList->add( new Layer() );	

	if( !currentLayer ) //first layer!
		currentLayer = layerList->at( layerID );

	//get the needed layer	
	return layerList->at( layerID );
}

bool Render::hasLayer( int layerID )
{
	LayerList* layerList = &positiveLayers;
	if( layerID < 0 )
	{
		layerID = -layerID - 1; //flip and shift by 1
		layerList = &negativeLayers;
	}
	
	return layerList->size() > abs( layerID );
}

void Render::addRenderable( Renderable* s, int layerID )
{				
	//get the needed layer	
	Layer* layer = getLayer( layerID );
	
#ifndef DOJO_FORCE_WHOLE_RENDERSTATE_COMMIT
	
	//insert this object in the place where the distances from its neighbours are a minimum.	
	uint bestIndex = 0;
	uint bestDistanceSum = 0xffffffff;
	
	uint distance;
	uint lastDistance = firstRenderState->getDistance( s );
	for( int i = 0; i < layer->size(); ++i )
	{
		distance = layer->at(i)->getDistance( s );
		if( distance + lastDistance < bestDistanceSum )
		{
			bestDistanceSum = distance + lastDistance;
			bestIndex = i;
		}
		
		lastDistance = distance;
	}
		
	s->_notifyRenderInfo( this, layerID, bestIndex );

	layer->add( s, bestIndex );
#else
	
	s->_notifyRenderInfo( this, layerID, layer->size() );
	
	//append at the end
	layer->add( s );    
	
#endif 
}

void Render::removeRenderable( Renderable* s )
{	
	if( hasLayer( s->getLayer() ) )
	{
		getLayer( s->getLayer() )->remove( s );
		s->_notifyRenderInfo( NULL, 0, 0 );
	}

	if( s == currentRenderState )
	{
		//firstRenderState->commitChanges( currentRenderState );
		currentRenderState = firstRenderState;
	}
}

void Render::addViewport( Viewport* v )
{
	DEBUG_ASSERT( v, "cannot add a null vieport" );

	mViewportList.add( v );
}

void Render::setInterfaceOrientation( Orientation o )		
{	
	renderOrientation = o;
	
	static float orientations[] = 	{ 0, 180, 90, -90 };
	
	renderRotation = orientations[ (uint)renderOrientation ] + orientations[ (uint)deviceOrientation ];
	
	//compute matrix
	mRenderRotation = glm::mat4_cast( Quaternion( Vector( 0,0, Math::toRadian( renderRotation )  ) ) );
}

void Render::renderElement( Viewport* viewport, Renderable* s )
{
	DEBUG_ASSERT( frameStarted, "Tried to render an element but the frame wasn't started" );
	DEBUG_ASSERT( viewport, "Rendering requires a Viewport to be set" );

	frameVertexCount += s->getMesh()->getVertexCount();
	frameTriCount += s->getMesh()->getTriangleCount();
	//each renderable is a single batch
	++frameBatchCount;
	
	//change the renderstate
	currentRenderState = s;
	
	currentState.world = s->getWorldTransform();
	currentState.worldView = currentState.view * currentState.world;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( glm::value_ptr( currentState.worldView ) );
	
#ifdef DOJO_SHADERS_AVAILABLE
	if( s->getShader() )
	{
		currentState.worldViewProjection = currentState.projection * currentState.worldView;
		s->getShader()->use( s );
	}
	else
		glUseProgram( 0 );
#endif
		
	//I'm not sure this actually makes sense
#ifndef USING_OPENGLES
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT );
#endif
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, (float*)(&s->color) );
	
	glEnable( GL_COLOR_MATERIAL );

	s->commitChanges( currentRenderState );

	Mesh* m = currentRenderState->getMesh();

	GLenum mode;
	switch( m->getTriangleMode() )
	{
		case Mesh::TM_LIST:         mode = GL_TRIANGLES;        break;
		case Mesh::TM_STRIP:    	mode = GL_TRIANGLE_STRIP;   break;
		case Mesh::TM_LINE_STRIP:   mode = GL_LINE_STRIP;       break;
		case Mesh::TM_LINE_LIST:	mode = GL_LINES;			break;
	}

	if( !m->isIndexed() )
		glDrawArrays( mode, 0, m->getVertexCount() );
	else
		glDrawElements( mode, m->getIndexCount(), m->getIndexGLType(), 0 );  //on OpenGLES, we have max 65536 indices!!!

#ifndef DOJO_DISABLE_VAOS
	glBindVertexArray( 0 );
#endif

#ifdef DOJO_SHADERS_AVAILABLE
	//HACK //TODO remove fixed function pipeline (it breaks if generic arrays are set)
	if( s->getShader() )
	{
		for( auto& attr : s->getShader()->getAttributes() )
			glDisableVertexAttribArray( attr.second.location );
	}
#endif
}

void Render::renderLayer( Viewport* viewport, Layer* list )
{
	if( !list->size() || !list->visible )
		return;
	
#ifdef DOJO_WIREFRAME_AVAILABLE
	setWireframe( list->wireframe );
#endif

	//make state changes
	if( list->depthCheck )	glEnable( GL_DEPTH_TEST );
	else					glDisable( GL_DEPTH_TEST );

	//set projection state
	currentState.projection = mRenderRotation * (list->projectionOff ? viewport->getOrthoProjectionTransform() : viewport->getPerspectiveProjectionTransform());
	
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( glm::value_ptr( currentState.projection ) );
	
	//we don't want different layers to be depth-checked together?
	if( list->depthClear )
		glClear( GL_DEPTH_BUFFER_BIT );

	currentLayer = list;
	
	if( list->lightingOn )
	{		
		glEnable( GL_LIGHTING );
		//enable or disable lights - TODO no need to do this each time, use an assigned slot system.
		int i = 0;
		for( ; i < lights.size(); ++i )
		{
			lights[i]->bind( i, currentState.view );
			
			if( !lights[i]->hasAmbient() )
				glLightfv( GL_LIGHT0 + i, GL_AMBIENT, (float*)&defaultAmbient );
		}
	}
	else
	{
		glDisable( GL_LIGHTING );

		for( int i = 0; i < lights.size(); ++i )
			glDisable( GL_LIGHT0 + i );
	}
	
	Renderable* s;

	//2D layer
	if( list->projectionOff )
	{
		for( int i = 0; i < list->size(); ++i )
		{
			s = list->at(i);
			
			//HACK use some 2D culling
			if( s->isVisible() )
				renderElement( viewport, s );
		}
	}

	//3D layer
	else
	{
		for( int i = 0; i < list->size(); ++i )
		{
			s = list->at(i);

			s->_notifyCulled( !viewport->isContainedInFrustum( s ) );
			
			if( s->isVisible() && s->isInView() )
				renderElement( viewport, s );
		}
	}
}

void Render::renderViewport( Viewport* viewport )
{
	DEBUG_ASSERT( viewport, "Cannot render with a null viewport" );

	Texture* rt = viewport->getRenderTarget();

	if( rt )
		rt->bindAsRenderTarget( true ); //TODO guess if this viewport doesn't render 3D layers to save memory?
	else
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glFrontFace( rt ? GL_CW : GL_CCW ); //invert vertex winding when inverting the view

    currentState.targetDimension.x = (float)(rt ? rt->getWidth() : width);
    currentState.targetDimension.y = (float)(rt ? rt->getHeight() : height);
    
	glViewport(0, 0, (GLsizei) currentState.targetDimension.x, (GLsizei)currentState.targetDimension.y);

	//clear the viewport
	glClearColor( 
		viewport->getClearColor().r, 
		viewport->getClearColor().g, 
		viewport->getClearColor().b, 
		viewport->getClearColor().a );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	currentState.view = viewport->getViewTransform();
	currentState.viewDirection = viewport->getWorldDirection();

	frameStarted = true;

	if( viewport->getVisibleLayers().empty() ) //use the custom layer ordering/visibility
	{
		//first render from the most negative to -1
		if( negativeLayers.size() > 0 )
		{	
			for( int i = negativeLayers.size()-1; i >= 0; --i )
				renderLayer( viewport, negativeLayers[i] );
		}

		//then from 0 to the most positive
		for( int i = 0; i < positiveLayers.size(); ++i )
			renderLayer( viewport, positiveLayers[i] );
	}
	else  //using the default layer ordering/visibility
	{
		for( auto& layer : viewport->getVisibleLayers() )
		{
			Layer* l = layer < 0 ? negativeLayers[-layer] : positiveLayers[layer];
			renderLayer( viewport, l );
		}
	}
}

void Render::render()
{
	DEBUG_ASSERT( !frameStarted, "Tried to start rendering but the frame was already started" );

	//HACK is this even needed? could be slow on windows
	platform->acquireContext();

	frameVertexCount = frameTriCount = frameBatchCount = 0;
	frameStarted = true;

	//render all the viewports
	for( auto viewport : mViewportList )
		renderViewport( viewport );

	//end the frame
	platform->present();

	frameStarted = false;
}