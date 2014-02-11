#import "GLView.h"
#include <OpenGL/gl.h>

#include "Vector2.h"
#include "Quantize.h"

static NSTimer *timer = nil;


static Quantize* quantize = new Quantize();
static bool isInitialized = false;

@implementation GLView

-(void) drawRect: (NSRect) bounds
{
    //[self setNeedsDisplay:YES];
    
    if( ! isInitialized) {
        quantize->initialize(
            [[[self window] contentView] bounds].size.width,
            [[[self window] contentView] bounds].size.height
        );
        
        isInitialized = true;
    }

    quantize->update(1.0f/60.0f);
    
}


- (void)mouseDown:(NSEvent *)theEvent {
    quantize->camera->onClick();
}

- (void)mouseMoved:(NSEvent *) event {
    CGPoint touchPosition = [event locationInWindow];

    Furiosity::Vector2 v(touchPosition.x, touchPosition.y);
    
    quantize->camera->onMove(v);
}

- (void)mouseDragged:(NSEvent *)theEvent {
    //quantize->onMove(<#const Furiosity::Vector2 &location#>);
    //printf("drag, it is.\n");
}

- (void) scrollWheel: (NSEvent*) event {
    Furiosity::Vector2 v([event deltaX], [event deltaY]);
    
    quantize->camera->onScroll(v);
}

- (void)keyDown:(NSEvent *)theEvent {
    if ([theEvent modifierFlags]) {
        NSString *theKey = [theEvent charactersIgnoringModifiers];
        unichar keyChar = 0;
        if ( [theKey length] == 0 )
            return;
        if ( [theKey length] == 1 ) {
            keyChar = [theKey characterAtIndex:0];
            quantize->camera->onKey(keyChar);
        }
    }
}

- (void)keyUp:(NSEvent *)theEvent {
    if ([theEvent modifierFlags]) {
        NSString *theKey = [theEvent charactersIgnoringModifiers];
        unichar keyChar = 0;
        if ( [theKey length] == 0 )
            return;
        if ( [theKey length] == 1 ) {
            keyChar = [theKey characterAtIndex:0];
            quantize->camera->onKeyDown(keyChar);
        }
    }
}

/*- (void)mouseDown:(NSEvent *)theEvent;
- (void)rightMouseDown:(NSEvent *)theEvent;
- (void)otherMouseDown:(NSEvent *)theEvent;
- (void)mouseUp:(NSEvent *)theEvent;
- (void)rightMouseUp:(NSEvent *)theEvent;
- (void)otherMouseUp:(NSEvent *)theEvent;
- (void)mouseMoved:(NSEvent *)theEvent;
- (void)mouseDragged:(NSEvent *)theEvent;
- (void)scrollWheel:(NSEvent *)theEvent;
- (void)rightMouseDragged:(NSEvent *)theEvent;
- (void)otherMouseDragged:(NSEvent *)theEvent;
- (void)mouseEntered:(NSEvent *)theEvent;
- (void)mouseExited:(NSEvent *)theEvent;
- (void)keyDown:(NSEvent *)theEvent;
- (void)keyUp:(NSEvent *)theEvent;*/


- (void)prepareOpenGL {
    [[self window] setAcceptsMouseMovedEvents: YES];
    [[self window] makeFirstResponder:self];
    
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