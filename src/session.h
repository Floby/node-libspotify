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
#include <node_buffer.h>
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
        Session (v8::Handle<v8::Object> buffer);
        virtual ~Session ();
        void initConfig();
        void initCallbacks();
        void scheduleProcessingEvents(unsigned int timeout);


        static void onLoggedIn(sp_session* spsession, sp_error error);
        static void onMainThreadNotified(sp_session* spsession);
        static void Idler(uv_idle_t* idler_handle, int status);

        static v8::Handle<v8::Value> Login(const v8::Arguments& args);

        static v8::Handle<v8::Value> New(const v8::Arguments& args);

        sp_session_config spconfig;
        sp_session_callbacks spcallbacks;
        sp_session* spsession;

        uv_timer_t process_events_handle;
        uv_timer_t timer_handle;
        uv_idle_t   idler_handle;

        uint8_t* g_appkey;
        size_t g_appkey_size;

        bool processing_scheduled;
        unsigned int do_schedule_processing_events;
    };

}

#endif /* NSP_SESSION_H */
