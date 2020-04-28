"""
Define Coordinate class

 Revised BSD License
 Copyright Semtech Corporation 2020. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Semtech corporation nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

from math import cos, sqrt, pi


class CoordinateException(Exception):
    pass


class CoordinateStringMalformedException(CoordinateException):
    def __init__(self, malformed_string):
        self.malformed_string = malformed_string

    def __str__(self):
        return "Error while attempting to convert string '{}' to Coordinate object".format(
            self.malformed_string
        )


class Coordinate:
    def __init__(self, latitude, longitude, altitude):
        self.latitude = latitude
        self.longitude = longitude
        self.altitude = altitude

    @staticmethod
    def from_string(coordinate_string):
        # coordinate_string should be formatted as 'xx.xxxxx,yy.yyyyyy,z.zzzzzz'
        try:
            lat_str, long_str, alt_str = coordinate_string.split(",")
        except ValueError:
            raise CoordinateStringMalformedException(coordinate_string)

        def to_float(val_string):
            return float(val_string.strip())

        return Coordinate(to_float(lat_str), to_float(long_str), to_float(alt_str))

    def to_string(self):
        def to_string(val_float):
            return "{:.8}".format(val_float)

        return ",".join(
            [to_string(val) for val in [self.latitude, self.longitude, self.altitude]]
        )

    def __sub__(self, other):
        """ Return the distance in meter between self and other
        """
        R = 6400e3

        def degree_to_radian(deg):
            return deg * pi / 180

        DY = degree_to_radian(self.latitude - other.latitude) * R
        DX = (
            degree_to_radian(self.longitude - other.longitude)
            * R
            * cos(degree_to_radian(other.latitude))
        )

        return sqrt((DY ** 2) + (DX ** 2))

    def __str__(self):
        return "Coordinate<lat:{}, long:{}, alt:{}>".format(
            self.latitude, self.longitude, self.altitude
        )

    @staticmethod
    def from_nmea_format(latitude_nmea, longitude_nmea, altitude_nmea):
        latitude_decimal_degree = Coordinate.nmea_format_to_deciman_degree(
            latitude_nmea
        )
        longitude_decimal_degree = Coordinate.nmea_format_to_deciman_degree(
            longitude_nmea
        )
        return Coordinate(
            latitude=latitude_decimal_degree,
            longitude=longitude_decimal_degree,
            altitude=altitude_nmea,
        )

    @staticmethod
    def nmea_format_to_deciman_degree(nmea_format):
        degree = int(nmea_format / 100)
        minute = nmea_format - (100 * degree)
        return Coordinate.degree_minutes_to_decimal_degree(degree=degree, minute=minute)

    @staticmethod
    def degree_minutes_to_decimal_degree(degree, minute):
        """ Convert a degree, minute, fraction of minute format into decimal degree
        """
        return degree + (minute / 60)
