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

#include <nan.h>
#include <node.h>
#include <v8.h>
#include "common.h"

void InitAll(Local<Object> target) {
  // initializing all modules
  nsp::init_album(target);
  nsp::init_artist(target);
  nsp::init_link(target);
  nsp::init_player(target);
  nsp::init_search(target);
  nsp::init_session(target);
  nsp::init_track(target);
  nsp::init_playlistcontainer(target);
  nsp::init_playlist(target);
}

NODE_MODULE(spotify, InitAll);
