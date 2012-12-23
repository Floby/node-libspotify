/*
 * =====================================================================================
 *
 *       Filename:  binding.cc
 *
 *    Description:  Node bindings for libspotify
 *
 *        Version:  1.0
 *        Created:  16/12/2012 15:04:18
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
#include "common.h"

extern "C" {
    void init (v8::Handle<v8::Object> target)
    {
        v8::HandleScope scope;

        // initializing all modules
        nsp::init_session(target);
        nsp::init_search(target);
    }
}


NODE_MODULE(libspotify, init);
