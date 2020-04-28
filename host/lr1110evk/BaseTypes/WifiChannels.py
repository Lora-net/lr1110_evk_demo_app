"""
Define Wi-Fi channel and Wi-Fi channel list classes

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


class WifiChannel:
    """ Wifi Channel

    Represents a wifi channel with its name and frequency

    Attributes:
        name (str): The name of the channel
        frequency_mhz (int): The center frequency of the channel [MHz]

    """

    def __init__(self, channel_name, channel_frequency_mhz):
        self.name = channel_name
        self.frequency_mhz = channel_frequency_mhz

    def __str__(self):
        return "{}".format(self.name)

    def __repr__(self):
        return "{}: {}".format(self.name, self.frequency_mhz)


class WifiChannelsException(Exception):
    """ Base exception for WifiChannel class

    """

    pass


class WifiChannelsNameUnknownException(WifiChannelsException):
    """Exception describing an attempt to get a channel which name is unknown

    Attributes:
        channel_name (str): The unknown channel name

    """

    def __init__(self, channel_name):
        self.channel_name = channel_name

    def __str__(self):
        return "Channel name unknown: '{}'".format(self.channel_name)


class WifiChannels:
    """ Lists all wifi channels

    Attributes:
        WIFI_CHANNELS ([WifiChannel]): The list of all Wifi channels
    """

    WIFI_CHANNELS = [
        WifiChannel("CHANNEL_{}".format(index), freq)
        for index, freq in enumerate(list(range(2412, 2473, 5)) + [2484], 1)
    ]

    @staticmethod
    def get_channel_from_name(name):
        """ Search a channel object from its name

        Raises a WifiChannelsNameUnknownException exception if name
        cannot be found.

        Args:
            name (str): The name of the channel to search

        Returns:
            WifiChannel: The Wifi channel object with the corresponding name

        """
        try:
            return [chan for chan in WifiChannels.WIFI_CHANNELS if chan.name == name][0]
        except IndexError:
            raise WifiChannelsNameUnknownException(name)
