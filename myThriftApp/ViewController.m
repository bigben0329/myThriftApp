//
//  ViewController.m
//  thrifttest2
//
//  Created by 平野 聡 on 12/01/05.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <TSocketClient.h>
#import <TBinaryProtocol.h>
#import "ViewController.h"
#import "idl.h"

@implementation ViewController
@synthesize textField;
@synthesize textView;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Talk to a server via socket, using a binary protocol
    TSocketClient *transport = [[TSocketClient alloc] initWithHostname:@"localhost" port:7911];
    TBinaryProtocol *protocol = [[TBinaryProtocol alloc] initWithTransport:transport strictRead:YES strictWrite:YES];
    server = [[BulletinBoardClient alloc] initWithProtocol:protocol];
}

- (void)viewDidUnload {
    [self setTextField:nil];
    [self setTextView:nil];
    [super viewDidUnload];
}

- (IBAction)addPressed:(id)sender {
    Message *msg = [[Message alloc] init];
    msg.text = textField.text;
    msg.date = [[NSDate date] description];
    
    [server add:msg];       // send data
    
    NSArray *array = [server get];    // receive data
    NSMutableString *s = [NSMutableString stringWithCapacity:1000];
    for (Message *m in array)
        [s appendFormat:@"%@ %@\n", m.date, m.text];
    textView.text = s;
}

- (BOOL)textFieldShouldReturn:(UITextField*)aTextField {
    [aTextField resignFirstResponder];
    return YES;
}
@end
