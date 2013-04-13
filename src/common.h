/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  Common utilities for bindings
 *
 *        Version:  1.0
 *        Created:  19/12/2012 12:14:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Florent Jaby (FJ), florent.jaby@gmail.com
 *        Company:  Florent Jaby
 *
 * =====================================================================================
 */

#ifndef NSP_COMMON_H
#define NSP_COMMON_H

#include <string.h>
#include <libspotify/api.h>
#include <node.h>
#include <node_buffer.h>
#include <assert.h>


/**
 * In a C++ function called from JS, takes a spotify error code, tests if not OK and throws an exception
 * with the appropriate error message
 */
#define NSP_THROW_IF_ERROR(error) if(error != SP_ERROR_OK) {return v8::ThrowException( \
            v8::Exception::Error(v8::String::New(sp_error_message( error )))\
        );}

/**
 * Reads a value at the given key and returns a C boolean value
 * @param o: the JS object on which to read at the given key
 * @param name: the key to read from
 * @return the boolean value read from the object
 */
inline bool NSP_BOOL_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    assert(value->IsBoolean());
    return value->BooleanValue();
}

/**
 * Reads a value at the given key and returns a C int value
 * @param o: the JS object on which to read at the given key
 * @param name: the key to read from
 * @return the int value read from the object
 */
inline int NSP_INT_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    assert(value->IsNumber());
    assert(value->IsUint32());
    return value->Int32Value();
}

/**
 * Reads a value at the given key and returns a C string value
 * NOTE: this function allocates the needed space for the string
 * it is the responsibility of the caller to free this pointer
 * @param o: the JS object on which to read at the given key
 * @param name: the key to read from
 * @return the string value read from the object
 */
inline char* NSP_STRING_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    if(value->IsNull()) {
        return NULL;
    }
    assert(value->IsString());

    char* v = new char[value->ToString()->Length()+1];
    strcpy(v, *(v8::String::AsciiValue(value)));
    return v;
}

/**
 * Reads a value at the given key and returns a Node buffer object
 * @param o: the JS object on which to read at the given key
 * @param name: the key to read from
 * @return the node buffer object read from the object
 */
inline char* NSP_BUFFER_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    assert(node::Buffer::HasInstance(value));
    return node::Buffer::Data(value->ToObject());
}

/**
 * Reads a value at the given key and returns a C int value which is the
 * length of a Node buffer object
 * @param o: the JS object on which to read at the given key
 * @param name: the key to read from
 * @return the length of the buffer read from the object
 */
inline int NSP_BUFFERLENGTH_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    assert(node::Buffer::HasInstance(value));
    return node::Buffer::Length(value->ToObject());
}

/**
 * Stands for Node+Spotify
 * namespace in which I declare most of the stuff I need for these bindings
 */
namespace nsp {

    /**
     * A function to use as a JS function that does nothing and returns this
     */
    v8::Handle<v8::Value> JsNoOp(const v8::Arguments&);

    /**
     * init the session related functions to the target module exports
     */
    void init_session(v8::Handle<v8::Object> target);
    /**
     * init the search related functions to the target module exports
     */
    void init_search(v8::Handle<v8::Object> target);
    /**
     * init the track related functions to the target module exports
     */
    void init_track(v8::Handle<v8::Object> target);
    /**
     * init the player related functions to the target module exports
     */
    void init_player(v8::Handle<v8::Object> target);
    /**
     * init the album related functions to the target module exports
     */
    void init_album(v8::Handle<v8::Object> target);
    /**
     * init the artist related functions to the target module exports
     */
    void init_artist(v8::Handle<v8::Object> target);
    /**
     * init the link related functions to the target module exports
     */
    void init_link(v8::Handle<v8::Object> target);
    /**
     * init the playlistcontainer related functions to the target module exports
     */
    void init_playlistcontainer(v8::Handle<v8::Object> target);
	/**
	 * init the playlist related functions to the target module exports
	 */
    void init_playlist(v8::Handle<v8::Object> target);
    
    /**
     * This utility class allows to keep track of a C pointer that we attached
     * to a JS object. It differs from node's ObjectWrap in the fact that it
     * does not need a constructor and both attributes are public.
     * Node's ObjectWrap is useful to wrap C++ classes whereas this class is useful
     * to wrap C structs. THIS CLASS DOES NOT MANAGE C MEMORY ALLOCATION
     */
    template <typename T>
    class ObjectHandle {
        public:
            /**
             * @constructor
             * Create a new ObjectHandle object with the given name
             * the name can be used later to identify the wrapped objects
             */
            ObjectHandle(const char* name);

            /**
             * Utility function to retrieve an ObjectHandle from a JS object
             * @param obj, the JS Object in which the ObjectHandle was wrapped
             */
            static ObjectHandle<T>* Unwrap(v8::Handle<v8::Value> obj);

            /**
             * A pointer to the C struct (most often) that we want to wrap
             * We do not allocate this
             */
            T* pointer;

            /**
             *  The JS Object that we set our pointer in
             *  We do create this one
             */
            v8::Persistent<v8::Object> object;
            
            /**
             * Get the name of the ObjectHandle that we gave it during instanciation
             */
            char* GetName() {
                return *(v8::String::Utf8Value(name_));
            }

        protected:
        private:
            v8::Persistent<v8::String> name_;

            /**
             * Empty function to set as constructor for an FunctionTemplate
             * @deprecated
             */
            v8::Handle<v8::Value> New(const v8::Arguments& args) {
                v8::HandleScope scope;
                // do nothing;
                return args.This();
            }
    };

    template <typename T>
    ObjectHandle<T>::ObjectHandle(const char* name) : pointer(NULL) {
        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(JsNoOp);
        name_ = v8::Persistent<v8::String>::New(
            v8::String::NewSymbol(name == NULL ? "CObject" : name)
        );

        tpl->SetClassName(name_);
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        v8::Local<v8::ObjectTemplate> otpl = tpl->InstanceTemplate();
        object = v8::Persistent<v8::Object>::New(otpl->NewInstance());

        object->SetPointerInInternalField(0, this);
    }
    
    template <typename T>
    ObjectHandle<T>* ObjectHandle<T>::Unwrap(v8::Handle<v8::Value> obj) {
        assert(obj->IsObject());
        v8::Handle<v8::Object> handle = obj->ToObject();
        ObjectHandle<T>* ptr = node::ObjectWrap::Unwrap<ObjectHandle<T> >(handle);
        return ptr;
    }
}

#endif /* NSP_COMMON_H */
