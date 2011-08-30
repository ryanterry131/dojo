/*
 *  Object.h
 *  NinjaTraining
 *
 *  Created by Tommaso Checchi on 4/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Object_h__
#define Object_h__

#include "dojo_common_header.h"

#include <stack>

#include "Vector.h"
#include "Array.h"
#include "dojomath.h"

namespace Dojo {
	
	class GameState;
	class Renderable;
	
	class Object 
	{
	public:
		
		typedef Dojo::Array< Object* > ChildList;
		
		bool dispose;

		bool inheritAngle;
		
		Vector position, speed, angle, rotationSpeed, scale;
		
		float* customMatrix;
		
		Object( GameState* parentLevel, const Vector& pos, const Vector& bbSize  );
		
		virtual ~Object()
		{
			if(childs)
				destroyAllChilds();
		}
		
		virtual void reset()
		{
			active = true;
			speed.x = speed.y = 0;
			
			angle = Vector::ZERO;
			rotationSpeed = Vector::ZERO;
			
			updateWorldPosition();
		}		
		
		void updateWorldPosition();
		
		inline void setSize( float x, float y )
		{						
			DEBUG_ASSERT( x >= 0 && y >= 0 );
						
			size.x = x;
			size.y = y;
			halfSize.x = size.x * 0.5f;
			halfSize.y = size.y * 0.5f;
		}
		
		inline void setSize( const Vector& bbSize )
		{
			setSize( bbSize.x, bbSize.y );
		}
				
		inline void setActive( bool a )		{	active = a;	}
				
		inline const Vector& getSize()		{	return size;	}			
		inline const Vector& getHalfSize()	{	return halfSize;}	
		
		inline GameState* getGameState()	{	return gameState;	}

		inline const Vector& getWorldPosition()
		{
			return worldPosition;
		}

		///returns the world position of the given local point
		inline Vector getWorldPosition( const Vector& localPos )
		{
			Vector pos = localPos;

			pos.scale( scale );
			pos.pitch( worldRotation.x );
			pos.yaw( worldRotation.y );

			return pos + worldPosition;
		}

		inline Vector getLocalPosition( const Vector& worldPos )
		{
			Vector localPos = worldPos;

			localPos -= worldPosition;
			localPos.roll( -worldRotation.z );
			localPos.yaw( -worldRotation.y );
			localPos.pitch( -worldRotation.x );
			localPos.scale( Vector( 1.f/scale.x, 1.f/scale.y, 1.f/scale.z ) );

			return localPos;
		}

		inline const Vector& getWorldRotation()
		{
			return worldRotation;
		}

		inline Vector getWorldDirection()
		{
			return worldRotation.directionFromAngle();
		}
						
		inline bool isActive()				{	return active;	}
		
		inline bool hasChilds()
		{
			return childs != NULL && childs->size() > 0;
		}
		
		//TODO use real transforms
		inline const Vector& getWorldMax()				{	return max;	}
		inline const Vector& getWorldMin()				{	return min;	}

		inline Object* getChild( int i )				
		{
			DEBUG_ASSERT( hasChilds() );
			DEBUG_ASSERT( childs->size() > i );
			
			return childs->at( i );
		}
		
		inline int getChildNumber()
		{
			return (childs) ? childs->size() : 0;
		}
		
		void addChild( Object* o );
		void addChild( Renderable* o, uint layer, bool clickable = false );

		void removeChild( Object* o );
		
		void collectChilds();
		
		///completely destroys all the childs of this object
		void destroyAllChilds();
		
		void updateChilds( float dt );
		
		inline bool collidesWith( const Vector& MAX, const Vector& MIN )
		{			
			return Math::AABBsCollide( getWorldMax(), getWorldMin(), MAX, MIN );
		}
		
		inline bool collidesWith( Object * t )
		{			
			DEBUG_ASSERT( t );

			return collidesWith( t->getWorldMax(), t->getWorldMin() );
		}
		
		virtual void onAction( float dt );

		virtual void onDestruction()
		{

		}

		void _notifyParent( Object* p )
		{
			DEBUG_ASSERT( p != parent );

			parent = p;
		}
				
	protected:		
		
		GameState* gameState;
		
		Vector size, halfSize, max, min;
		
		Vector worldPosition;
		Vector worldRotation;
				
		bool active;

		Object* parent;
		ChildList* childs;
	};
}

#endif