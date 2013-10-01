/*
 * =====================================================================================
 *
 *       Filename:  imagecallbacks.cc
 *
 *    Description:  callbacks for the image subsystem
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

static void cb_image_loaded(sp_image *sp_obj, void *userdata) {
    ObjectHandle<sp_image>* image = (ObjectHandle<sp_image>*) userdata;
    Handle<Value> cbv = image->object->Get(String::New("image_loaded"));

    if(cbv->IsFunction()) {
        Handle<Function> callback = Local<Function>(Function::Cast(*cbv));
        callback->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
}
