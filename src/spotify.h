/*
 * =====================================================================================
 *
 *       Filename:  spotify.h
 *
 *    Description:  Spotify object declaration
 *
 *        Version:  1.0
 *        Created:  16/12/2012 15:09:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Florent Jaby (FJ), florent.jaby@gmail.com
 *        Company:  Florent Jaby
 *
 * =====================================================================================
 */

#include <node.h>
#include <v8.h>
#include <stdio.h>

v8::Handle<v8::Value> bidule(const v8::Arguments& args);

v8::Handle<v8::Value> hello(const v8::Arguments& args);
