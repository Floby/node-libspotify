/*
 * =====================================================================================
 *
 *       Filename:  search.cc
 *
 *    Description: bindings to the spotify search submodule
 *
 *        Version:  1.0
 *        Created:  23/12/2012 16:59:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Florent Jaby (FJ), florent.jaby@gmail.com
 *        Company:  Florent Jaby
 *
 * =====================================================================================
 */


#include "common.h"

using namespace v8;
using namespace nsp;

/**
 * Spotify callback when a search query completed
 */
static void on_search_complete(sp_search* result, void* userdata) {
  ObjectHandle<sp_search>* search = static_cast<ObjectHandle<sp_search>* >(userdata);
  Local<Value> cbv = Nan::New(search->object)->Get(Nan::GetCurrentContext(), Nan::New<String>("on_search_complete").ToLocalChecked()).ToLocalChecked();
  if(!cbv->IsFunction()) {
    return;
  }
  Nan::Callback *cb = new Nan::Callback(cbv.As<v8::Function>());

  const unsigned int argc = 2;
  sp_error error = sp_search_error(result);
  Local<Value> err = Nan::Null();
  if(error != SP_ERROR_OK) {
    err = Exception::Error(Nan::New<String>(sp_error_message(error)).ToLocalChecked());
  }
  Local<Value> argv[argc] = { Nan::New<Value>(err), Nan::New<Object>(search->object) };
  cb->Call(argc, argv);

  return;
}

/**
 * JS search_create implementation. calls sp_search_create
 */
NAN_METHOD(Search_Create) {


  // check arguments sanity
  assert(info.Length() == 10);
  assert(info[0]->IsObject());
  assert(info[1]->IsString());
  assert(info[2]->IsNumber());
  assert(info[3]->IsNumber());
  assert(info[4]->IsNumber());
  assert(info[5]->IsNumber());
  assert(info[6]->IsNumber());
  assert(info[7]->IsNumber());
  assert(info[8]->IsNumber());
  assert(info[9]->IsNumber());

  // unwrap the session handle from the given object
  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[0]);

  // create the new handle for the sp_search we are creating
  ObjectHandle<sp_search>* search = new ObjectHandle<sp_search>("sp_search");
  String::Utf8Value query(info[1]);

  // actually call sp_search_create
  search->pointer = sp_search_create(
      session->pointer,
      *query,
      info[2]->ToNumber()->Int32Value(),
      info[3]->ToNumber()->Int32Value(),
      info[4]->ToNumber()->Int32Value(),
      info[5]->ToNumber()->Int32Value(),
      info[6]->ToNumber()->Int32Value(),
      info[7]->ToNumber()->Int32Value(),
      info[8]->ToNumber()->Int32Value(),
      info[9]->ToNumber()->Int32Value(),
      SP_SEARCH_STANDARD,
      &on_search_complete,
      search
  );

  info.GetReturnValue().Set(Nan::New(search->object));
}

/**
 * JS search_num_tracks implementation.
 */
NAN_METHOD(Search_Num_Tracks) {


  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  ObjectHandle<sp_search>* search = ObjectHandle<sp_search>::Unwrap(info[0]);
  int num = sp_search_num_tracks(search->pointer);

  info.GetReturnValue().Set(Nan::New<Number>(num));
}

/**
 * JS search_track implementation. gets a track a the given index in a search result
 */
NAN_METHOD(Search_Track) {


  // test arguments sanity
  assert(info.Length() == 2);
  assert(info[0]->IsObject());
  assert(info[1]->IsNumber());

  // gets sp_search pointer from given object
  ObjectHandle<sp_search>* search = ObjectHandle<sp_search>::Unwrap(info[0]);
  int index = info[1]->ToNumber()->Int32Value();

  // check index is within search results range
  assert(index >= 0);
  assert(index < sp_search_num_tracks(search->pointer));

  // create new handle for this track
  ObjectHandle<sp_track>* track = new ObjectHandle<sp_track>("sp_track");
  track->pointer = sp_search_track(search->pointer, index);

  info.GetReturnValue().Set(Nan::New(track->object));
}

void nsp::init_search(Local<Object> target) {
  Nan::Export(target, "search_create", Search_Create);
  Nan::Export(target, "search_num_tracks", Search_Num_Tracks);
  Nan::Export(target, "search_track", Search_Track);
}
