//
//  ViewController.h
//  myThriftApp
//
//  Created by 平野 聡 on 12/02/22.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class BulletinBoardClient;

@interface ViewController : UIViewController <UITextFieldDelegate> {
    BulletinBoardClient *server;
}
@property (strong, nonatomic) IBOutlet UITextField *textField;
@property (strong, nonatomic) IBOutlet UITextView *textView;
- (IBAction)addPressed:(id)sender;
@end
