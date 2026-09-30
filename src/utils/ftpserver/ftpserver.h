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
#ifndef _FTPSERVER_H
    #define _FTPSERVER_H

    #include <stdint.h>

    #define FTPSERVER_USER      "ftp"
    #define FTPSERVER_PASSWORD  "ftp"

    /**
     * @brief start the ftp server with the given credentials
     * 
     * @param user  ftp username (ignored; always ftp/ftp)
     * @param pass  ftp password (ignored; always ftp/ftp)
     */
    void ftpserver_start( const char *user, const char *pass );

    /**
     * @brief must be called frequently from the main loop to service
     *        the ftp control and data connections. without this, the
     *        client will time out and the server may crash.
     */
    void ftpserver_handle( void );

#endif // _FTPSERVER_H
