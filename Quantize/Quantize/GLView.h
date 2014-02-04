//
//  GLView.h
//  Quantize
//
//  Created by Gerard Meier on 03/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#pragma once


#import <Cocoa/Cocoa.h>

@interface GLView : NSOpenGLView<NSWindowDelegate> {
//@interface GLView : NSOpenGLView {
}

- (void) drawRect: (NSRect) bounds;

@end

