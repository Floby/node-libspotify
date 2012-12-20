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

#include <assert.h>


#define NSP_THROW_IF_ERROR(error) if(error != SP_ERROR_OK) {return v8::ThrowException( \
            v8::Exception::Error(v8::String::New(sp_error_message( error )))\
        );}

inline bool NSP_BOOL_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    assert(value->IsBoolean());
    return value->BooleanValue();
}

inline int NSP_INT_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    assert(value->IsNumber());
    assert(value->IsUint32());
    return value->Int32Value();
}

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

inline char* NSP_BUFFER_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    assert(node::Buffer::HasInstance(value));
    return node::Buffer::Data(value->ToObject());
}

inline int NSP_BUFFERLENGTH_KEY(v8::Handle<v8::Object> o, const char* name) {
    assert(o->IsObject());
    v8::Handle<v8::Value> value = o->Get(v8::String::New(name));
    assert(node::Buffer::HasInstance(value));
    return node::Buffer::Length(value->ToObject());
}

namespace nsp {
    v8::Handle<v8::Value> JsNoOp(const v8::Arguments&);

    template <typename T>
    class ObjectHandle {
        public:
            ObjectHandle(const char* name);
            //~ObjectHandle();

            static ObjectHandle<T>* Unwrap(v8::Handle<v8::Value> obj);

            T* pointer;
            v8::Persistent<v8::Object> object;
            
            char* GetName() {
                return *(v8::String::Utf8Value(name_));
            }

        protected:
        private:
            v8::Persistent<v8::String> name_;

            v8::Handle<v8::Value> New(const v8::Arguments& args) {
                v8::HandleScope scope;
                // do nothing;
                return args.This();
            }
    };

    template <typename T>
    ObjectHandle<T>::ObjectHandle(const char* name = NULL) : pointer(NULL) {
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
