/*
 * =====================================================================================
 *
 *       Filename:  session.h
 *
 *    Description:  Bindings for the sp_session subsystem
 *
 *        Version:  1.0
 *        Created:  16/12/2012 21:37:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Florent Jaby (FJ), florent.jaby@gmail.com
 *        Company:  Florent Jaby
 *
 * =====================================================================================
 */


#ifndef NSP_SESSION_H
#define NSP_SESSION_H
#include <node.h>
#include <libspotify/api.h>

namespace nsp {

    class Session :public node::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> target);
    
    private:
        Session ();
        virtual ~Session ();

        static v8::Handle<v8::Value> New(const v8::Arguments& args);

        sp_session_config config;
        sp_session_callbacks callbacks;
        sp_session* spsession;
    };

}

#endif /* NSP_SESSION_H */
