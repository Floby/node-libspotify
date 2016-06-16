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
  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(info[0]);

  // actually call sp_album_is_loaded
  bool loaded = sp_album_is_loaded(album->pointer);

  info.GetReturnValue().Set(Nan::New<Boolean>(loaded));
}

/**
 * JS album_name implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Name) {
  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(info[0]);

  const char* name = sp_album_name(album->pointer);

  info.GetReturnValue().Set(Nan::New<v8::String>(name).ToLocalChecked());
}

/**
 * JS album_year implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Year) {
  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(info[0]);

  const int year = sp_album_year(album->pointer);

  info.GetReturnValue().Set(Nan::New<Number>(year));
}

/**
 * JS album_type implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Type) {
  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(info[0]);

  sp_albumtype type = sp_album_type(album->pointer);
  v8::Local<v8::Value> res;
  switch (type) {
    case SP_ALBUMTYPE_ALBUM:
      res = Nan::New<v8::String>("ALBUM").ToLocalChecked();
      break;
    case SP_ALBUMTYPE_SINGLE:
      res = Nan::New<v8::String>("SINGLE").ToLocalChecked();
      break;
    case SP_ALBUMTYPE_COMPILATION:
      res = Nan::New<v8::String>("COMPILATION").ToLocalChecked();
      break;
    case SP_ALBUMTYPE_UNKNOWN:
    default:
      res = Nan::New<v8::String>("UNKNOWN").ToLocalChecked();
      break;
  }

  info.GetReturnValue().Set(res);
}

/**
 * JS album_artist implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Artist) {
  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(info[0]);

  sp_artist* spartist = sp_album_artist(album->pointer);
  ObjectHandle<sp_artist>* artist = new ObjectHandle<sp_artist>("sp_artist");
  artist->pointer = spartist;

  info.GetReturnValue().Set(Nan::New(artist->object));
}

/**
 * Callback for sp_image_add_load_callback in Album_Cover().
 * It calls the passed JS function callback and passes the raw image data as
 * parameter as soon as the regarding image loading process has finished.
 */
void cb_image_loaded_album(sp_image *image, void *userdata) {
  Nan::Callback * callback = static_cast<Nan::Callback *>(userdata);
  size_t image_size;
  const void *image_data = sp_image_data(image, &image_size);

  // Create a C++ world slow buffer:
  Local<Object> slowBuffer = Nan::NewBuffer(const_cast<char *>(static_cast<const char *>(image_data)), image_size).ToLocalChecked();

  // Get the Buffer constructor from the JavaScript world:
  Local<Object> globalObj = Nan::GetCurrentContext()->Global();
  Local<Function> bufferConstructor = globalObj->Get(Nan::GetCurrentContext(), Nan::New<v8::String>("Buffer").ToLocalChecked()).ToLocalChecked().As<v8::Function>();
  Local<Value> constructorinfo[3] = { slowBuffer, Nan::New<Number>(image_size), Nan::New<Number>(0) };

  // Create a JavaScript buffer using the slow buffer:
  Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorinfo);

  // Pass everything to the JavaScript callback:
  const unsigned argc = 1;
  Local<Value> argv[argc] = { actualBuffer };
  callback->Call(argc, argv);

  // Clean up:
  delete callback;
  sp_image_release(image);
}

/**
 * JS album_cover implementation. Gets the albums image
 */
NAN_METHOD(Album_Cover) {
  // test arguments sanity
  assert(info.Length() == 4);
  assert(info[0]->IsObject());  // sp_session
  assert(info[1]->IsObject());  // sp_album
  assert(info[2]->IsNumber());  // sp_image_size
  assert(info[3]->IsFunction()); // callback after cover image was loaded

  ObjectHandle<sp_session> *session = ObjectHandle<sp_session>::Unwrap(info[0]);
  ObjectHandle<sp_album> *album = ObjectHandle<sp_album>::Unwrap(info[1]);
  Local<Integer> requestedImageSize = info[2].As<Integer>();
  Nan::Callback *callback = new Nan::Callback(info[3].As<Function>());

  sp_image_size imageSize = static_cast<sp_image_size>(requestedImageSize->Uint32Value());
  const byte *imageId = sp_album_cover(album->pointer, imageSize);

  if(imageId) {
    sp_image *image = sp_image_create(session->pointer, imageId);
    sp_image_add_load_callback(image, &cb_image_loaded_album, callback);
    info.GetReturnValue().Set(Nan::New<Boolean>(true));
  } else {
    info.GetReturnValue().Set(Nan::New<Boolean>(false));
  }
}

void nsp::init_album(Local<Object> target) {
  Nan::Export(target, "album_is_loaded", Album_Is_Loaded);
  Nan::Export(target, "album_name", Album_Name);
  Nan::Export(target, "album_year", Album_Year);
  Nan::Export(target, "album_type", Album_Type);
  Nan::Export(target, "album_artist", Album_Artist);
  Nan::Export(target, "album_cover", Album_Cover);
  target->Set(Nan::GetCurrentContext(), Nan::New<v8::String>("SP_IMAGE_SIZE_SMALL").ToLocalChecked(), Nan::New<Int32>(static_cast<int>(SP_IMAGE_SIZE_SMALL)));
  target->Set(Nan::GetCurrentContext(), Nan::New<v8::String>("SP_IMAGE_SIZE_NORMAL").ToLocalChecked(), Nan::New<Int32>(static_cast<int>(SP_IMAGE_SIZE_NORMAL)));
  target->Set(Nan::GetCurrentContext(), Nan::New<v8::String>("SP_IMAGE_SIZE_LARGE").ToLocalChecked(), Nan::New<Int32>(static_cast<int>(SP_IMAGE_SIZE_LARGE)));
}
