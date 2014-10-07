//
//  XYZToDoListViewController.h
//  ToDoList
//
//  Created by gerryyang on 14-10-6.
//  Copyright (c) 2014年 gerryyang. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface XYZToDoListViewController : UITableViewController

@property NSMutableArray *toDoItems;

- (IBAction)unwindToList:(UIStoryboardSegue *)segue;



@end
