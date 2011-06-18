/*
 *  Buffer.h
 *  Ninja Training
 *
 *  Created by Tommaso Checchi on 8/10/10.
 *  Copyright 2010 none. All rights reserved.
 *
 */

#ifndef Buffer_h__
#define Buffer_h__

#include "dojo/dojo_common_header.h"


#include "dojo/BaseObject.h"

namespace Dojo 
{
	class ResourceGroup;
	
	class Buffer : public BaseObject
	{
	public:
		
		Buffer( ResourceGroup* creatorGroup, const String& path ) :
		creator( creatorGroup ),
		filePath( path ),
		loaded( false ),
		size( 0 )
		{
			
		}
		
		virtual ~Buffer()
		{
			/*if( isLoaded() )
				unload();*/
		}
		
		virtual bool load()=0;
		virtual void unload()=0;
		
		virtual void bind( uint index )
		{
			DEBUG_TODO;
		}
		
		inline bool isLoaded()
		{
			return loaded;
		}
		
		inline uint getByteSize()
		{
			DEBUG_ASSERT( size );
			
			return size;
		}
		
		inline ResourceGroup* getCreator()
		{			
			return creator;
		}
		
		inline const String& getFilePath()
		{
			return filePath;
		}
		
	protected:
		
		ResourceGroup* creator;
		
		bool loaded;
		uint size;
		
		String filePath;
	};
}

#endif