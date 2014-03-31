#import "GLView.h"
#include "Tools.h"

#include "Vector2.h"
#include "Quantize.h"

static NSTimer *timer = nil;

static double lastUpdate = GetTiming();

static Quantize* quantize = Quantize::getInstance();
static bool isInitialized = false;

@implementation GLView


- (void) awakeFromNib {
    //printf("awakeFromNib- nibnibnibnibnooob\n");
    
    NSOpenGLPixelFormatAttribute attr[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        
        NSOpenGLPFAAccelerated,
        //NSOpenGLPFANoRecovery,
       // NSOpenGLPFABackingStore,
        NSOpenGLPFADoubleBuffer,
        
        NSOpenGLPFAColorSize, 32, // color buffer
        //NSOpenGLPFADepthSize, 24, // depth buffer
        //NSOpenGLPFAAlphaSize, 8,

        NSOpenGLPFASampleBuffers, 0, // # of sample buffers
        NSOpenGLPFASamples, 0, // # of samples

        
        0 // Null byte, indicates end of structure.
    };
    
    NSOpenGLPixelFormat *glPixForm = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
    
    [self setPixelFormat: glPixForm];
    
}

-(void) drawRect: (NSRect) bounds
{
    double startTime = GetTiming();

    //[self setNeedsDisplay:YES];
    
    if( ! isInitialized) {
        quantize->initialize(
            [[[self window] contentView] bounds].size.width,
            [[[self window] contentView] bounds].size.height
        );
        
        isInitialized = true;
    }

    quantize->update(startTime - lastUpdate);
        
    lastUpdate = startTime;
    
    [[self openGLContext] flushBuffer];
}


- (void)mouseDown:(NSEvent *)theEvent {
    quantize->camera.onClick();
}

- (void)mouseMoved:(NSEvent *) event {
    CGPoint touchPosition = [event locationInWindow];

    Furiosity::Vector2 v(touchPosition.x, touchPosition.y);
    
    quantize->camera.onMove(v);
}

- (void)mouseDragged:(NSEvent *)theEvent {
    //quantize->onMove(<#const Furiosity::Vector2 &location#>);
    //printf("drag, it is.\n");
}

- (void) scrollWheel: (NSEvent*) event {
    Furiosity::Vector2 v([event deltaX], [event deltaY]);
    
    quantize->camera.onScroll(v);
}

- (void)keyDown:(NSEvent *)theEvent {
    if ([theEvent modifierFlags]) {
        NSString *theKey = [theEvent charactersIgnoringModifiers];
        unichar keyChar = 0;
        if ( [theKey length] == 0 )
            return;
        if ( [theKey length] == 1 ) {
            keyChar = [theKey characterAtIndex:0];
        
            if(keyChar == 0x20) {
                quantize->shootPhotons();
            } else if(keyChar == 0x6e) {
                quantize->useLambertian = 1 - quantize->useLambertian;
                printf("useLambertian = %d\n", quantize->useLambertian);
                
             } else if(keyChar == 0x6d) {
                quantize->showPhotons = 1 - quantize->showPhotons;
                printf("showPhotons = %d\n", quantize->showPhotons);
                
              } else if(keyChar == 0x62) {
                quantize->useANN = 1 - quantize->useANN;
                
                printf("useANN = %d\n", quantize->useANN);
                
            } else {
                quantize->camera.onKey(keyChar);
            }
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
            
            if(keyChar == 0x20) {
                
            } else {
                quantize->camera.onKeyUp(keyChar);
            }
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