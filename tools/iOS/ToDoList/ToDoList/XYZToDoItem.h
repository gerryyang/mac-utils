//
//  XYZToDoItem.h
//  ToDoList
//
//  Created by gerryyang on 14-10-7.
//  Copyright (c) 2014年 gerryyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface XYZToDoItem : NSObject

@property NSString *itemName;
@property BOOL completed;
@property (readonly) NSDate *creationDate;

@end
