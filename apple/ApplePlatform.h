//
//  ApplePlatform.h
//  dojo
//
//  Created by Tommaso Checchi on 5/16/11.
//  Copyright 2011 none. All rights reserved.
//


#ifndef __ApplePlatform_h
#define __ApplePlatform_h

#include "../include/dojo/dojo_common_header.h"

#include "../include/dojo/Platform.h"

#ifdef __OBJC__
	#import <Foundation/NSAutoreleasePool.h>
#endif

namespace Dojo
{
	
#ifndef __OBJC__
	class NSAutoreleasePool;
	class NSString;
#endif
	
    class ApplePlatform : public Platform
    {
    public:
		
        ApplePlatform( const Table& config );
		
		virtual ~ApplePlatform();
		
		virtual void initialise()=0;        
		virtual void step( float dt );
		
		virtual String getAppDataPath();
		virtual String getRootPath();
				
		virtual bool loadPNGContent( void*& imageData, const String& path, int& width, int& height );		
		
		virtual void openWebPage( const String& site );
        
    protected:
		
		void _createApplicationDirectory();
		
		//these always exists because .cpp and .mm compiling this header could get different sizes for the class!!!
       	NSAutoreleasePool* pool;
    };
}

#endif