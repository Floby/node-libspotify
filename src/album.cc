/*
 * =====================================================================================
 *
 *       Filename:  album.cc
 *
 *    Description:  bindings for the album subsystem
 *
 *        Version:  1.0
 *        Created:  03/01/2013 15:18:14
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
 * JS album_is_loaded implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Is_Loaded) {
    NanScope();

  // test arguments sanity
  assert(args.Length() == 1);
  assert(args[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(args[0]);

  // actually call sp_album_is_loaded
  bool loaded = sp_album_is_loaded(album->pointer);

    NanReturnValue(NanNew<Boolean>(loaded));
}

/**
 * JS album_name implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Name) {
    NanScope();

  // test arguments sanity
  assert(args.Length() == 1);
  assert(args[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(args[0]);

  const char* name = sp_album_name(album->pointer);

    NanReturnValue(NanNew<String>(name));
}

/**
 * JS album_artist implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Artist) {
    NanScope();

  // test arguments sanity
  assert(args.Length() == 1);
  assert(args[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(args[0]);

  sp_artist* spartist = sp_album_artist(album->pointer);
  ObjectHandle<sp_artist>* artist = new ObjectHandle<sp_artist>("sp_artist");
  artist->pointer = spartist;

    NanReturnValue(artist->object);
}

/**
 * JS album_cover implementation. Gets the albums image
 */
static Handle<Value> Album_Cover(const Arguments& args) {
  HandleScope scope;

  // test arguments sanity
  assert(args.Length() == 4);
  assert(args[0]->IsObject());  // sp_session
  assert(args[1]->IsObject());  // sp_album
  assert(args[2]->IsNumber());  // sp_image_size
  assert(args[3]->IsFunction()); // callback after cover image was loaded

  ObjectHandle<sp_session> *session = ObjectHandle<sp_session>::Unwrap(args[0]);
  ObjectHandle<sp_album> *album = ObjectHandle<sp_album>::Unwrap(args[1]);
  Handle<Integer> requestedImageSize = Local<Integer>::Cast(args[2]);
  Handle<Function> callback = Persistent<Function>::New(Handle<Function>::Cast(args[3]));

  sp_image_size imageSize = static_cast<sp_image_size>(requestedImageSize->Uint32Value());
  const byte *imageId = sp_album_cover(album->pointer, imageSize);

  if(imageId) {
    sp_image *image = sp_image_create(session->pointer, imageId);
    sp_image_add_load_callback(image, &cb_image_loaded_album, *callback);
    return scope.Close(Boolean::New(true));
  } else {
    return scope.Close(Boolean::New(false));
  }

}

void nsp::init_album(Handle<Object> target) {
  NODE_SET_METHOD(target, "album_is_loaded", Album_Is_Loaded);
  NODE_SET_METHOD(target, "album_name", Album_Name);
  NODE_SET_METHOD(target, "album_year", Album_Year);
  NODE_SET_METHOD(target, "album_type", Album_Type);
  NODE_SET_METHOD(target, "album_artist", Album_Artist);
  NODE_SET_METHOD(target, "album_cover", Album_Cover);
  target->Set(v8::String::NewSymbol("SP_IMAGE_SIZE_SMALL"), v8::Int32::New(static_cast<int>(SP_IMAGE_SIZE_SMALL)), ReadOnly);
  target->Set(v8::String::NewSymbol("SP_IMAGE_SIZE_NORMAL"), v8::Int32::New(static_cast<int>(SP_IMAGE_SIZE_NORMAL)), ReadOnly);
  target->Set(v8::String::NewSymbol("SP_IMAGE_SIZE_LARGE"), v8::Int32::New(static_cast<int>(SP_IMAGE_SIZE_LARGE)), ReadOnly);
}
