/* sha1.h

Copyright (c) 2005 Michael D. Leonhard

http://tamale.net/

This file is licensed under the terms described in the
accompanying LICENSE file.

modified by Kamil 'konserw' Strzempowicz
*/
/**
    kOferta - system usprawniajacy proces ofertowania
    Copyright (C) 2011  Kamil 'konserw' Strzempowicz, konserw@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef SHA1_HEADER
typedef unsigned int Uint32;

class QString;

class SHA1
{
	private:
		// fields
		Uint32 H0, H1, H2, H3, H4;
		unsigned char bytes[64];
		int unprocessedBytes;
		Uint32 size;
		void process();

                unsigned char* digest;
	public:
		SHA1();
		~SHA1();
		void addBytes( const char* data, int num );
		unsigned char* getDigest();
		// utility methods
                static Uint32 lrot( Uint32 x, int bits );
                static void storeBigEndianUint32( unsigned char* byte, Uint32 num );
                //added by konserw
                QString toQstring();
                void fromQstring(QString);

};

QString hash(QString);                                  //convinient hash function, added by konserw

#define SHA1_HEADER
#endif
