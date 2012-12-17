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
#include <stdint.h>
#include <stdlib.h>

namespace nsp {

    class Session :public node::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> target);
        unsigned int number;
        void processEvents();
    
    private:
        Session ();
        virtual ~Session ();
        void initAppKey();
        void initConfig();
        void initCallbacks();


        static void onLoggedIn(sp_session* spsession, sp_error error);
        static void onMainThreadNotified(sp_session* spsession);

        static v8::Handle<v8::Value> New(const v8::Arguments& args);

        sp_session_config spconfig;
        sp_session_callbacks spcallbacks;
        sp_session* spsession;

        uv_timer_t process_events_handle;
        uv_timer_t timer_handle;

        uint8_t* g_appkey;
        size_t g_appkey_size;
    };

}

#endif /* NSP_SESSION_H */
