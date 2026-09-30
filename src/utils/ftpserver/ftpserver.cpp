/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "hardware/powermgm.h"
#include "utils/ftpserver/ftpserver.h"

#ifdef NATIVE_64BIT
#else
    #include <Arduino.h>
    #include <SPIFFS.h>
    #include <FTPServer.h>

    FTPServer *ftpSrv = NULL;
#endif

void ftpserver_start( const char *user, const char *pass ) {
#ifdef NATIVE_64BIT

#else
    (void)user;
    (void)pass;

    if ( !ftpSrv ) {
        if ( !SPIFFS.begin( false ) ) {
            log_e("SPIFFS mount failed, FTP server not started");
            return;
        }

        ftpSrv = new FTPServer( SPIFFS );
        if ( ftpSrv ) {
            /*
             * Always use ftp/ftp. Saved wificfg.json may still contain the
             * legacy TTWatch/password pair from older firmware.
             */
            ftpSrv->begin( FTPSERVER_USER, FTPSERVER_PASSWORD );
            log_i("FTP server started, user/password: %s/%s", FTPSERVER_USER, FTPSERVER_PASSWORD );
        }
        else {
            log_e("start ftp server failed");
        }
    }
#endif
}

void ftpserver_handle( void ) {
#ifdef NATIVE_64BIT

#else
    if ( ftpSrv ) {
        ftpSrv->handleFTP();
    }
#endif
}
