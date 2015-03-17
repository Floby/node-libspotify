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
 * JS album_year implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Year) {
  NanScope();

  // test arguments sanity
  assert(args.Length() == 1);
  assert(args[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(args[0]);

  const int year = sp_album_year(album->pointer);

  NanReturnValue(NanNew<Number>(year));
}

/**
 * JS album_type implementation. checks if a given album is loaded
 */
NAN_METHOD(Album_Type) {
  NanScope();

  // test arguments sanity
  assert(args.Length() == 1);
  assert(args[0]->IsObject());

  // gets sp_album pointer from given object
  ObjectHandle<sp_album>* album = ObjectHandle<sp_album>::Unwrap(args[0]);

  sp_albumtype type = sp_album_type(album->pointer);
  Handle<Value> res;
  switch (type) {
    case SP_ALBUMTYPE_ALBUM:
      res = NanNew<String>("ALBUM");
      break;
    case SP_ALBUMTYPE_SINGLE:
      res = NanNew<String>("SINGLE");
      break;
    case SP_ALBUMTYPE_COMPILATION:
      res = NanNew<String>("COMPILATION");
      break;
    case SP_ALBUMTYPE_UNKNOWN:
    default:
      res = NanNew<String>("UNKNOWN");
      break;
  }

  NanReturnValue(res);
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
 * Callback for sp_image_add_load_callback in Album_Cover().
 * It calls the passed JS function callback and passes the raw image data as
 * parameter as soon as the regarding image loading process has finished.
 */
void cb_image_loaded_album(sp_image *image, void *userdata) {
  NanCallback * callback = static_cast<NanCallback *>(userdata);
  size_t image_size;
  const void *image_data = sp_image_data(image, &image_size);

  // Create a C++ world slow buffer:
  Local<Object> slowBuffer = NanNewBufferHandle(static_cast<const char *>(image_data), image_size);

  // Get the Buffer constructor from the JavaScript world:
  Local<Object> globalObj = NanGetCurrentContext()->Global();
  Local<Function> bufferConstructor = globalObj->Get(NanNew<String>("Buffer")).As<Function>();
  Handle<Value> constructorArgs[3] = { slowBuffer, NanNew<Number>(image_size), NanNew<Number>(0) };

  // Create a JavaScript buffer using the slow buffer:
  Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);

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
  NanScope();

  // test arguments sanity
  assert(args.Length() == 4);
  assert(args[0]->IsObject());  // sp_session
  assert(args[1]->IsObject());  // sp_album
  assert(args[2]->IsNumber());  // sp_image_size
  assert(args[3]->IsFunction()); // callback after cover image was loaded

  ObjectHandle<sp_session> *session = ObjectHandle<sp_session>::Unwrap(args[0]);
  ObjectHandle<sp_album> *album = ObjectHandle<sp_album>::Unwrap(args[1]);
  Handle<Integer> requestedImageSize = args[2].As<Integer>();
  NanCallback *callback = new NanCallback(args[3].As<Function>());

  sp_image_size imageSize = static_cast<sp_image_size>(requestedImageSize->Uint32Value());
  const byte *imageId = sp_album_cover(album->pointer, imageSize);

  if(imageId) {
    sp_image *image = sp_image_create(session->pointer, imageId);
    sp_image_add_load_callback(image, &cb_image_loaded_album, callback);
    NanReturnValue(NanNew<Boolean>(true));
  } else {
    NanReturnValue(NanNew<Boolean>(false));
  }
}

void nsp::init_album(Handle<Object> target) {
  NODE_SET_METHOD(target, "album_is_loaded", Album_Is_Loaded);
  NODE_SET_METHOD(target, "album_name", Album_Name);
  NODE_SET_METHOD(target, "album_year", Album_Year);
  NODE_SET_METHOD(target, "album_type", Album_Type);
  NODE_SET_METHOD(target, "album_artist", Album_Artist);
  NODE_SET_METHOD(target, "album_cover", Album_Cover);
  target->Set(NanNew<String>("SP_IMAGE_SIZE_SMALL"), NanNew<Int32>(static_cast<int>(SP_IMAGE_SIZE_SMALL)));
  target->Set(NanNew<String>("SP_IMAGE_SIZE_NORMAL"), NanNew<Int32>(static_cast<int>(SP_IMAGE_SIZE_NORMAL)));
  target->Set(NanNew<String>("SP_IMAGE_SIZE_LARGE"), NanNew<Int32>(static_cast<int>(SP_IMAGE_SIZE_LARGE)));
}
