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

@end
