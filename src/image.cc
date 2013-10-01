/*
 * =====================================================================================
 *
 *       Filename:  image.cc
 *
 *    Description:  bindings for the image subsystem
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Linus Unnebäck, linus@folkdatorn.se
 *        Company:  LinusU AB
 *
 * =====================================================================================
 */


#include "common.h"

using namespace v8;
using namespace nsp;

static Handle<Value> Image_Is_Loaded(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    ObjectHandle<sp_image>* image = ObjectHandle<sp_image>::Unwrap(args[0]);
    bool loaded = sp_image_is_loaded(image->pointer);

    return scope.Close(Boolean::New(loaded));
}

static Handle<Value> Image_Data(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    size_t image_size;
    ObjectHandle<sp_image>* image = ObjectHandle<sp_image>::Unwrap(args[0]);
    const void *image_data = sp_image_data(image->pointer, &image_size);

    // Create a C++ world slow buffer:
    node::Buffer *slowBuffer= node::Buffer::New(image_size);
    memcpy(node::Buffer::Data(slowBuffer), image_data, image_size);

    // Get the Buffer constructor from the JavaScript world:
    Local<Object> globalObj = Context::GetCurrent()->Global();
    Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
    Handle<Value> constructorArgs[3] = { slowBuffer->handle_, Integer::New(image_size), Integer::New(0) };

    // Create a JavaScript buffer using the slow buffer:
    Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);

    return scope.Close(actualBuffer);
}

static Handle<Value> Image_Relase(const Arguments& args) {
    HandleScope scope;

    assert(args.Length() == 1);
    assert(args[0]->IsObject());

    ObjectHandle<sp_image>* image = ObjectHandle<sp_image>::Unwrap(args[0]);
    sp_image_release(image->pointer);

    return scope.Close(Undefined());
}

void nsp::init_image(Handle<Object> target) {
    NODE_SET_METHOD(target, "image_data", Image_Data);
    NODE_SET_METHOD(target, "image_relase", Image_Relase);
    NODE_SET_METHOD(target, "image_is_loaded", Image_Is_Loaded);
}
