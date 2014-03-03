//
//  AppDelegate.h
//  Quantize
//
//  Created by Gerard Meier on 03/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GLView.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSScrollView *cameraControlsView;
@property (weak) IBOutlet NSTextField *nMonitor;
@property (weak) IBOutlet NSTextField *sigmaMonitor;
@property (weak) IBOutlet NSTextField *rangeMonitor;
@property (weak) IBOutlet NSTextField *lightsMonitor;

@end
