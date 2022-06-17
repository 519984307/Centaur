/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "AppleDirectory.h"
#import <Foundation/Foundation.h>

const char *getApplicationSupportDirectory()
{
    NSString *directory = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES) lastObject];
    return [directory cStringUsingEncoding:[NSString defaultCStringEncoding]];
}
