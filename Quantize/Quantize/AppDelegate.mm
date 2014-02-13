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

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    printf("applicationDidFinishLaunching\n");

}

- (IBAction)cameraSpeedSliderAction:(id)sender {
    float value = [sender floatValue] / 100.0f;
    quantize->camera->moveSpeed = value;
}

- (IBAction)cameraMouseSliderAction:(id)sender {
    float value = [sender floatValue] / 10000.0f;
    quantize->camera->mouseSpeed = value;
}

- (IBAction)cameraRollSliderAction:(id)sender {
    float value = [sender floatValue] / 1000.0f;
    quantize->camera->rollSpeed = value;
}

- (IBAction)cameraLockCheckBoxAction:(id)sender {
    bool value = [sender state] == NSOnState;
    //printf("%d", (int)value);
    quantize->camera->locked = value;
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
    float multiplier = 2.5f;
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
    float multiplier = 2.5f;
    float r = [color redComponent] * multiplier;
    float g = [color greenComponent] * multiplier;
    float b = [color blueComponent] * multiplier;
    float a = [color alphaComponent];
    quantize->lights[1].diffuse.r = r;
    quantize->lights[1].diffuse.g = g;
    quantize->lights[1].diffuse.b = b;
    quantize->lights[1].diffuse.a = a;
}

@end