/*
 * =====================================================================================
 *
 *       Filename:  spotify.cc
 *
 *    Description: Spotify bindings implementation 
 *
 *        Version:  1.0
 *        Created:  16/12/2012 15:42:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Florent Jaby (FJ), florent.jaby@gmail.com
 *        Company:  Florent Jaby
 *
 * =====================================================================================
 */

#include "spotify.h"

v8::Handle<v8::Value> bidule(const v8::Arguments& args) {
    printf("chose\n");
    return v8::Null();
}

v8::Handle<v8::Value> hello(const v8::Arguments& args) {
    v8::HandleScope scope;
    return scope.Close(v8::String::New("World"));
}
