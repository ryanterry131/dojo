//
//  StepCallback.m
//  dojo
//
//  Created by Tommaso Checchi on 5/15/11.
//  Copyright 2011 none. All rights reserved.
//

#import "StepCallback.h"

#include "Platform.h"

@implementation StepCallback

- (id)initWithPlatform:(Dojo::Platform*)targetPlatform
{
    self = [super init];
    if (self) {
        
		platform = targetPlatform;
		
		time = CFAbsoluteTimeGetCurrent();
    }
    
    return self;
}


- (void)stepCallback:(NSTimer *)timer
{
	CFTimeInterval deltaTime = CFAbsoluteTimeGetCurrent() - time;
	
	time = CFAbsoluteTimeGetCurrent();
	
	//HACK - use real elapsed time
	platform->step( deltaTime );
}

- (void)dealloc
{
    [super dealloc];
}

@end
