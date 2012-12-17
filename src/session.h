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


#include <node.h>

namespace "nsp" {

    class Session :public node::ObjectWrap {
    public:
        Session ();
        virtual ~Session ();
    
    private:
        sp_session_config spconfig;
        sp_session spsession;
    };

}
