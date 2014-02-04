#import "GLView.h"
#include <OpenGL/gl.h>

static NSTimer *timer = nil;


static float foo = 0;

static void drawAnObject ()
{
    glColor3f(1.0f, 0.85f, 0.35f);
    glTranslatef(foo, foo, foo);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(  0.0,  0.6, 0.0);
        glVertex3f( -0.2, -0.3, 0.0);
        glVertex3f(  0.2, -0.3 ,0.0);
    }
    glEnd();
    
    foo += 0.0001;
}

@implementation GLView

-(void) drawRect: (NSRect) bounds
{
    printf("drawRect\n");

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawAnObject();
    glFlush();
    
    [self setNeedsDisplay:YES];
}

- (void)prepareOpenGL {
    printf("prepareOpenGL\n");

    // Synchronize buffer swaps with vertical refresh rate
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


    // Update loop
    // Draw loop
    
    [self setNeedsDisplay:YES];
}


@end