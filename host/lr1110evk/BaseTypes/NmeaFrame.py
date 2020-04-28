"""
Define NMEA class

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

from pynmea2 import parse, ChecksumError
from pynmea2.nmea import ParseError


class NmeaException(Exception):
    pass


class NmeaWrongChecksumException(NmeaException):
    def __init__(self, raw_nmea):
        self._raw_nmea = raw_nmea
        super(NmeaWrongChecksumException, self).__init__()

    def __str__(self):
        return "Checksum is wrong or missing: '{}'".format(self._raw_nmea)


class NmeaFrameUnknownSentenceType(NmeaException):
    def __init__(self, unknown_sentence_type):
        self.unknown_sentence_type = unknown_sentence_type
        super(NmeaFrameUnknownSentenceType, self).__init__()

    def __str__(self):
        return "Unknown NMEA sentence type '{}' (known types: {})".format(
            self.unknown_sentence_type, NmeaFrame.KNOWN_SENTENCE_TYPES
        )


class NmeaParseError(NmeaException):
    def __init__(self, raw_nmea):
        self.raw_nmea = raw_nmea

    def __str__(self):
        return "Failed to parse NMEA sentence '{}'".format(self.raw_nmea)


class NmeaFrame:
    KNOWN_SENTENCE_TYPES = ["GGA"]

    def __init__(self, raw_str):
        self._raw_str = raw_str
        self._parsed_nmea = None

    def __str__(self):
        return self._raw_str.strip()

    def parse(self):
        try:
            self._parsed_nmea = parse(self._raw_str, check=True)
        except ChecksumError:
            raise NmeaWrongChecksumException(self._raw_str)
        except ParseError:
            raise NmeaParseError(self._raw_str)
        sentence_type = self._parsed_nmea.sentence_type
        NmeaFrame.check_known_sentence_type_or_raise_error(sentence_type)

    @staticmethod
    def check_known_sentence_type_or_raise_error(sentence_type):
        if sentence_type not in NmeaFrame.KNOWN_SENTENCE_TYPES:
            raise NmeaFrameUnknownSentenceType(sentence_type)

    @property
    def longitude(self):
        if self._parsed_nmea.lon_dir == "W":
            return -1 * float(self._parsed_nmea.lon)
        else:
            return float(self._parsed_nmea.lon)

    @property
    def latitude(self):
        return float(self._parsed_nmea.lat)

    @property
    def altitude(self):
        return float(self._parsed_nmea.altitude)

    @property
    def timestamp(self):
        return self._parsed_nmea.timestamp

    @property
    def raw_str(self):
        return self._raw_str

    @raw_str.setter
    def raw_str(self, value):
        self._raw_str = value
        self._parsed_nmea = None
