/*
 * =====================================================================================
 *
 *       Filename:  track.cc
 *
 *    Description:  Bindings for the tracks submodule
 *
 *        Version:  1.0
 *        Created:  24/12/2012 15:32:58
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
 * JS track_is_loaded implementation. checks if a given track is loaded
 */
NAN_METHOD(Track_Is_Loaded) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_track pointer from given object
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[0]);

  // actually call sp_track_is_loaded
  bool loaded = sp_track_is_loaded(track->pointer);

  info.GetReturnValue().Set(Nan::New<Boolean>(loaded));
}

/**
 * JS track_duration implementation. checks if a given track is loaded
 */
NAN_METHOD(Track_Duration) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_track pointer from given object
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[0]);

  // actually call sp_track_is_loaded
  int duration = sp_track_duration(track->pointer);

  info.GetReturnValue().Set(Nan::New<Number>(duration));
}

/**
 * JS track_num_artists implementation. checks if a given track is loaded
 */
NAN_METHOD(Track_Num_Artists) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_track pointer from given object
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[0]);

  int num_artists = sp_track_num_artists(track->pointer);

  info.GetReturnValue().Set(Nan::New<Number>(num_artists));
}

/**
 * JS track_name implementation. checks if a given track is loaded
 */
NAN_METHOD(Track_Name) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_track pointer from given object
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[0]);

  const char* name = sp_track_name(track->pointer);

  info.GetReturnValue().Set(Nan::New<String>(name).ToLocalChecked());
}

/**
 * JS track_artist implementation. checks if a given track is loaded
 */
NAN_METHOD(Track_Artist) {


  // test arguments sanity
  assert(info.Length() == 2);
  assert(info[0]->IsObject());
  assert(info[1]->IsNumber());

  // gets sp_track pointer from given object
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[0]);
  int index = info[1]->ToNumber()->Int32Value();
  assert(index >= 0);
  assert(index < sp_track_num_artists(track->pointer));

  sp_artist* spartist = sp_track_artist(track->pointer, index);
  ObjectHandle<sp_artist>* artist = new ObjectHandle<sp_artist>("sp_artist");
  artist->pointer = spartist;

  info.GetReturnValue().Set(Nan::New(artist->object));
}


/**
 * JS track_album implementation. checks if a given track is loaded
 */
NAN_METHOD(Track_Album) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_track pointer from given object
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[0]);

  sp_album* spalbum = sp_track_album(track->pointer);
  ObjectHandle<sp_album>* album = new ObjectHandle<sp_album>("sp_album");
  album->pointer = spalbum;

  info.GetReturnValue().Set(Nan::New(album->object));
}

/**
 * JS track_get_availability implementation. checks if a given track is loaded
 */
NAN_METHOD(Track_Get_Availability) {


  // test arguments sanity
  assert(info.Length() == 2);
  assert(info[0]->IsObject());
  assert(info[1]->IsObject());

  // gets sp_track pointer from given object
  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[0]);
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[1]);

  sp_availability availability = sp_track_get_availability(session->pointer, track->pointer);
  Local<Value> res;
  switch (availability) {
    case SP_TRACK_AVAILABILITY_AVAILABLE:
      res = Nan::New<String>("AVAILABLE").ToLocalChecked();
      break;
    case SP_TRACK_AVAILABILITY_NOT_STREAMABLE:
      res = Nan::New<String>("NOT_STREAMABLE").ToLocalChecked();
      break;
    case SP_TRACK_AVAILABILITY_BANNED_BY_ARTIST:
      res = Nan::New<String>("BANNED_BY_ARTIST").ToLocalChecked();
      break;
    case SP_TRACK_AVAILABILITY_UNAVAILABLE:
    default:
      res = Nan::New<String>("UNAVAILABLE").ToLocalChecked();
      break;
  }

  info.GetReturnValue().Set(res);
}

NAN_METHOD(Track_Is_Starred) {


  // test arguments sanity
  assert(info.Length() == 2);
  assert(info[0]->IsObject());
  assert(info[1]->IsObject());

  // get session and track pointers from object
  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[0]);
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[1]);

  bool starred = sp_track_is_starred(session->pointer, track->pointer);

  info.GetReturnValue().Set(Nan::New<Boolean>(starred));
}

NAN_METHOD(Track_Set_Starred) {


  // test arguments sanity
  assert(info.Length() == 3);
  assert(info[0]->IsObject());
  assert(info[1]->IsArray());
  assert(info[2]->IsBoolean());

  ObjectHandle<sp_session>* session = ObjectHandle<sp_session>::Unwrap(info[0]);

  Local<Array> arr = info[1].As<Array>();
  unsigned int length = arr->Length();
  sp_track* tracks[length];
  for(unsigned int i=0; i<length; ++i) {
    tracks[i] = ObjectHandle<sp_track>::Unwrap(arr->Get(i))->pointer;
  }

  sp_error error = sp_track_set_starred(session->pointer, tracks, length, info[2]->ToBoolean()->BooleanValue());
  NSP_THROW_IF_ERROR(error);

  info.GetReturnValue().SetUndefined();
}

/**
 * JS track_popularity implementation. checks if a given track is loaded
 */
NAN_METHOD(Track_Popularity) {


  // test arguments sanity
  assert(info.Length() == 1);
  assert(info[0]->IsObject());

  // gets sp_track pointer from given object
  ObjectHandle<sp_track>* track = ObjectHandle<sp_track>::Unwrap(info[0]);

  // actually call sp_track_popularity
  int popularity = sp_track_popularity(track->pointer);

  info.GetReturnValue().Set(Nan::New<Number>(popularity));
}

void nsp::init_track(Local<Object> target) {
  Nan::Export(target, "track_is_loaded", Track_Is_Loaded);
  Nan::Export(target, "track_duration", Track_Duration);
  Nan::Export(target, "track_num_artists", Track_Num_Artists);
  Nan::Export(target, "track_name", Track_Name);
  Nan::Export(target, "track_artist", Track_Artist);
  Nan::Export(target, "track_album", Track_Album);
  Nan::Export(target, "track_get_availability", Track_Get_Availability);
  Nan::Export(target, "track_is_starred", Track_Is_Starred);
  Nan::Export(target, "track_popularity", Track_Popularity);
  Nan::Export(target, "track_set_starred", Track_Set_Starred);
}
