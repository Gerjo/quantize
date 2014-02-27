//
//  AppDelegate.m
//  Quantize
//
//  Created by Gerard Meier on 03/02/14.
//  Copyright (c) 2014 Quantize. All rights reserved.
//

#import "AppDelegate.h"
#include "Quantize.h"

static NSTimer *timer = nil;
static Quantize* quantize = Quantize::getInstance();

@implementation AppDelegate
@synthesize cameraControlsView;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    printf("applicationDidFinishLaunching\n");
    
    //Give content to the controls text view.
    NSTextView *tV = [cameraControlsView documentView];
    [tV readRTFDFromFile:@"Quantize/Camera Controls.rtf"];
}

- (IBAction)cameraSpeedSliderAction:(id)sender {
    float value = [sender floatValue] / 100.0f;
    quantize->camera.moveSpeed = value;
}

- (IBAction)cameraMouseSliderAction:(id)sender {
    float value = [sender floatValue] / 10000.0f;
    quantize->camera.mouseSpeed = value;
}

- (IBAction)cameraRollSliderAction:(id)sender {
    float value = [sender floatValue] / 1000.0f;
    quantize->camera.rollSpeed = value;
}

- (IBAction)lights1XSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[0].position.x = value;
}
- (IBAction)lights1YSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[0].position.y = value;
}
- (IBAction)lights1ZSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[0].position.z = value;
}

- (IBAction)lights1ColorWellAction:(id)sender {
    NSColor *color = [sender color];
    float multiplier = 1.0f;
    float r = [color redComponent] * multiplier;
    float g = [color greenComponent] * multiplier;
    float b = [color blueComponent] * multiplier;
    float a = [color alphaComponent];
    quantize->lights[0].diffuse.r = r;
    quantize->lights[0].diffuse.g = g;
    quantize->lights[0].diffuse.b = b;
    quantize->lights[0].diffuse.a = a;
}

- (IBAction)lights2XSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[1].position.x = value;
}

- (IBAction)lights2YSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[1].position.y = value;
}

- (IBAction)lights2ZSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[1].position.z = value;
}

- (IBAction)lights2ColorWellAction:(id)sender {
    NSColor *color = [sender color];
    float multiplier = 1.0f;
    float r = [color redComponent] * multiplier;
    float g = [color greenComponent] * multiplier;
    float b = [color blueComponent] * multiplier;
    float a = [color alphaComponent];
    quantize->lights[1].diffuse.r = r;
    quantize->lights[1].diffuse.g = g;
    quantize->lights[1].diffuse.b = b;
    quantize->lights[1].diffuse.a = a;
}
- (IBAction)lights3XSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[2].position.x = value;
}
- (IBAction)lights3YSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[2].position.y = value;
}
- (IBAction)lights3ZSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[2].position.z = value;
}
- (IBAction)lights3ColorWellAction:(id)sender {
    NSColor *color = [sender color];
    float multiplier = 1.0f;
    float r = [color redComponent] * multiplier;
    float g = [color greenComponent] * multiplier;
    float b = [color blueComponent] * multiplier;
    float a = [color alphaComponent];
    quantize->lights[2].diffuse.r = r;
    quantize->lights[2].diffuse.g = g;
    quantize->lights[2].diffuse.b = b;
    quantize->lights[2].diffuse.a = a;
}
- (IBAction)lights4XSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[3].position.x = value;
}
- (IBAction)lights4YSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[3].position.y = value;
}
- (IBAction)lights4ZSliderAction:(id)sender {
    float value = [sender floatValue] - 50.0f;
    quantize->lights[3].position.z = value;
}
- (IBAction)lights4ColorWellAction:(id)sender {
    NSColor *color = [sender color];
    float multiplier = 1.0f;
    float r = [color redComponent] * multiplier;
    float g = [color greenComponent] * multiplier;
    float b = [color blueComponent] * multiplier;
    float a = [color alphaComponent];
    quantize->lights[3].diffuse.r = r;
    quantize->lights[3].diffuse.g = g;
    quantize->lights[3].diffuse.b = b;
    quantize->lights[3].diffuse.a = a;
}

- (IBAction)lerpSliderAction:(id)sender {
    //float value = [sender floatValue] / 100.0f;
    //quantize->kernelLerp = value;
}

- (IBAction)kernelPopupAction:(id)sender {
    //int value = (int)[sender indexOfSelectedItem];
    //quantize->kernelType = value;
}

@end
