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

#include <nan.h>
#include <libspotify/api.h>
#include <node.h>
#include <node_buffer.h>
#include <assert.h>

using v8::Function;
using v8::FunctionTemplate;
using v8::Local;
using v8::Null;
using v8::Number;
using v8::Value;
using v8::String;
using v8::Object;
using v8::ObjectTemplate;
using v8::Handle;
using v8::Persistent;


/**
 * In a C++ function called from JS, takes a spotify error code, tests if not OK and throws an exception
 * with the appropriate error message
 */
#define NSP_THROW_IF_ERROR(error) if(error != SP_ERROR_OK) {return v8::ThrowException( \
            v8::Exception::Error(NanNew<String>(sp_error_message( error )))\
        );}

/**
 * Reads a value at the given key and returns a C boolean value
 * @param o: the JS object on which to read at the given key
 * @param name: the key to read from
 * @return the boolean value read from the object
 */
inline bool NSP_BOOL_KEY(Handle<Object> o, const char* name) {
    assert(o->IsObject());
    Local<Value> value = o->Get(NanNew<String>(name));
    assert(value->IsBoolean());
    return value->BooleanValue();
}

/**
 * Reads a value at the given key and returns a C int value
 * @param o: the JS object on which to read at the given key
 * @param name: the key to read from
 * @return the int value read from the object
 */
inline int NSP_INT_KEY(Handle<Object> o, const char* name) {
    assert(o->IsObject());
    Local<Value> value = o->Get(NanNew<String>(name));
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
inline char* NSP_STRING_KEY(Handle<Object> o, const char* name) {
    assert(o->IsObject());
    Local<Value> value = o->Get(NanNew<String>(name));
    if(value->IsNull()) {
        return NULL;
    }
    assert(value->IsString());

    char* v = new char[value->ToString()->Length()+1];
    strcpy(v, *(String::Utf8Value(value)));
    return v;
}

/**
 * Reads a value at the given key and returns a Node buffer object
 * @param o: the JS object on which to read at the given key
 * @param name: the key to read from
 * @return the node buffer object read from the object
 */
inline char* NSP_BUFFER_KEY(Handle<Object> o, const char* name) {
    assert(o->IsObject());
    Local<Value> value = o->Get(NanNew<String>(name));
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
inline int NSP_BUFFERLENGTH_KEY(Handle<Object> o, const char* name) {
    assert(o->IsObject());
    Local<Value> value = o->Get(NanNew<String>(name));
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
    NAN_METHOD(JsNoOp);

    /**
     * init the session related functions to the target module exports
     */
    void init_session(Handle<Object> target);
    /**
     * init the search related functions to the target module exports
     */
    void init_search(Handle<Object> target);
    /**
     * init the track related functions to the target module exports
     */
    void init_track(Handle<Object> target);
    /**
     * init the player related functions to the target module exports
     */
    void init_player(Handle<Object> target);
    /**
     * init the album related functions to the target module exports
     */
    void init_album(Handle<Object> target);
    /**
     * init the artist related functions to the target module exports
     */
    void init_artist(Handle<Object> target);
    /**
     * init the link related functions to the target module exports
     */
    void init_link(Handle<Object> target);


    /**
     * This utility class allows to keep track of a C pointer that we attached
     * to a JS object. It differs from node's ObjectWrap in the fact that it
     * does not need a constructor and both attributes are public.
     * Node's ObjectWrap is useful to wrap C++ classes whereas this class is useful
     * to wrap C structs. THIS CLASS DOES NOT MANAGE C MEMORY ALLOCATION
     */
    template <typename T>
    class ObjectHandle: public node::ObjectWrap {
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
            static ObjectHandle<T>* Unwrap(Local<Value> obj);

            /**
             * A pointer to the C struct (most often) that we want to wrap
             * We do not allocate this
             */
            T* pointer;

            /**
             *  The JS Object that we set our pointer in
             *  We do create this one
             */
            Persistent<Object> object;
            
            /**
             * Get the name of the ObjectHandle that we gave it during instanciation
             */
            char* GetName() {
                return *(String::Utf8Value(name_));
            }

        protected:
        private:
            Persistent<String> name_;

            /**
             * Empty function to set as constructor for an FunctionTemplate
             * @deprecated
             */
            NAN_METHOD(New) {
                NanScope();
                // do nothing;
                NanReturnThis();
            }
    };

  template <typename T>
    ObjectHandle<T>::ObjectHandle(const char* name) : pointer(NULL) {
        Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(JsNoOp);
        NanAssignPersistent(name_, NanNew<String>(name == NULL ? "CObject" : name));

        tpl->SetClassName(NanNew(name_));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        Local<ObjectTemplate> otpl = tpl->InstanceTemplate();
        NanAssignPersistent(object, otpl->NewInstance());

        NanSetInternalFieldPointer(NanNew(object), 0, this);
    }
    
    template <typename T>
    ObjectHandle<T>* ObjectHandle<T>::Unwrap(Local<Value> obj) {
        assert(obj->IsObject());
        Local<Object> handle = obj->ToObject();
        ObjectHandle<T>* ptr = node::ObjectWrap::Unwrap<ObjectHandle<T> >(handle);
        return ptr;
    }
}

#endif /* NSP_COMMON_H */
