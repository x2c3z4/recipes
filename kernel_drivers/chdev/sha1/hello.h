/*
 * =====================================================================================
 *
 *       Filename:  hello.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年12月12日 21时20分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *        Company:
 *
 * =====================================================================================
 */
#ifndef _HELLO_H_
#define _HELLO_H_

#include <linux/semaphore.h>
#include <linux/cdev.h>


#define HELLO_DEVICE_NODE_NAME "hello"
#define HELLO_DEVICE_FILE_NAME "hello"
#define HELLO_DEVICE_PROC_NAME "hello"
#define HELLO_DEVICE_CLASS_NAME "hello"

struct hello_android_dev{
  int val;
  struct semaphore sem;
  struct cdev dev;
};

#endif
