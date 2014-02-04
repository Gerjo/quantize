#import "GLView.h"
#include <OpenGL/gl.h>

#include "Quantize.h"

static NSTimer *timer = nil;


static Quantize* quantize = new Quantize();
static bool isInitialized = false;

@implementation GLView

-(void) drawRect: (NSRect) bounds
{
    //[self setNeedsDisplay:YES];
    
    if( ! isInitialized) {
        quantize->initialize();
        isInitialized = true;
    }

    quantize->update(1.0f/60.0f);
    
}

- (void)prepareOpenGL {
    
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    timer = [NSTimer timerWithTimeInterval:1/60
               target:self 
               selector:@selector(timerEvent:) 
               userInfo:nil 
               repeats:YES];
    
    [[NSRunLoop mainRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];

}

- (void)timerEvent:(NSTimer *)t {
    [self setNeedsDisplay:YES];
}


@end