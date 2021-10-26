"""
Define KML file exporter class

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

import xml.etree.cElementTree as ET
import os


class kmlOutput:
    NS = "{http://www.opengis.net/kml/2.2}"
    COLOR_BLACK = "ff000000"
    COLOR_GREEN = "ff00aaff"
    COLOR_ORANGE = "ff00aa00"

    SCAN_TYPE_GNSS = 0
    SCAN_TYPE_REFERENCE_COORDINATES = 1
    SCAN_TYPE_WIFI = 2

    def __init__(self, name, filename):
        self._style_black = self._create_kml_style(self.COLOR_BLACK, 0.0)
        self._style_orange = self._create_kml_style(self.COLOR_GREEN, 0.0)
        self._style_green = self._create_kml_style(self.COLOR_ORANGE, 0.0)
        self._filename = filename

        # Create XML structure if it does not exists
        if not os.path.isfile(filename):
            self._create_file(filename)

        # Load XML
        self._kml = ET.parse(filename).getroot()
        self._doc = self._kml.find("Document")
        self._folder_top = self._doc.find('./Folder[name="Localisations"]')
        self._folder_reference = self._folder_top.find('./Folder[name="References"]')
        self._folder_wifi = self._folder_top.find('./Folder[name="Wi-Fi"]')
        self._folder_gnss = self._folder_top.find('./Folder[name="GNSS"]')

    def _create_file(self, filename):
        self._kml = ET.Element("kml")
        self._doc = ET.SubElement(self._kml, "Document")
        self._folder_top = self._add_kml_folder(
            self._doc,
            "Localisations",
        )
        self._folder_reference = self._add_kml_folder(self._folder_top, "References")
        self._folder_wifi = self._add_kml_folder(self._folder_top, "Wi-Fi")
        self._folder_gnss = self._add_kml_folder(self._folder_top, "GNSS")
        self.save()

    @staticmethod
    def _add_kml_folder(parent, name):
        folder = ET.SubElement(parent, "Folder")
        ET.SubElement(folder, "name").text = name
        return folder

    @staticmethod
    def _create_kml_style(icon_color=None, label_scale=None):
        style = ET.Element("Style")
        icon = ET.SubElement(style, "IconStyle")
        label = ET.SubElement(style, "LabelStyle")

        if icon_color is not None:
            ET.SubElement(icon, "color").text = icon_color
        if label_scale is not None:
            ET.SubElement(label, "scale").text = label_scale

        return style

    def __add_kml_point(
        self, name, folder, coordinates, style, metadata=None, timestamp=None
    ):
        placemark = ET.SubElement(folder, "Placemark", id="id_{}".format(name))

        # Add Timestamp information if available
        if timestamp:
            kml_timestamp = ET.SubElement(placemark, "TimeStamp")
            ET.SubElement(kml_timestamp, "when").text = timestamp.isoformat()

        placemark.append(style)
        ET.SubElement(placemark, "name").text = name
        if metadata is not None:
            placemark.append(metadata)

        point = ET.SubElement(placemark, "Point")
        ET.SubElement(point, "coordinates").text = "{},{},{}".format(
            coordinates.longitude, coordinates.latitude, coordinates.altitude
        )

    def add_point(self, name, localization_type, coordinates, metadata=None):
        if localization_type == kmlOutput.SCAN_TYPE_WIFI:
            extended_data = ET.Element("ExtendedData")
            if metadata:
                instant_scan = metadata[0].instant_scan
                # extended_data = ET.Element("ExtendedData")
                data = ET.SubElement(extended_data, "Data", name="date")
                ET.SubElement(data, "value").text = "{}".format(instant_scan)
                id_result = 0
                data = ET.SubElement(extended_data, "Data", name="count")
                ET.SubElement(data, "value").text = "{}".format(len(metadata))
                for result in metadata:
                    data = ET.SubElement(
                        extended_data,
                        "Data",
                        id="wifi_{}".format(id_result),
                        name="data",
                    )
                    ET.SubElement(data, "value").text = "{};{};{};{}".format(
                        result.mac_address,
                        result.rssi,
                        result.wifi_channel,
                        result.wifi_type,
                    )
                    id_result += 1
            else:
                instant_scan = None
            self.__add_kml_point(
                name,
                self._folder_wifi,
                coordinates,
                self._style_orange,
                extended_data,
                timestamp=instant_scan,
            )
        elif localization_type == kmlOutput.SCAN_TYPE_GNSS:
            extended_data = ET.Element("ExtendedData")
            if metadata:
                date = metadata[0].instant_scan.strftime("%Y-%m-%d %H:%M:%S")
                data = ET.SubElement(extended_data, "Data", name="date")
                ET.SubElement(data, "value").text = "{}".format(date)
                data = ET.SubElement(extended_data, "Data", name="count")
                ET.SubElement(data, "value").text = "{}".format(len(metadata))
                for index, nav in enumerate(metadata):
                    data = ET.SubElement(
                        extended_data,
                        "Data",
                        id="gnss_{}".format(index),
                        name="data",
                    )
                    ET.SubElement(data, "value").text = "{}".format(
                        ",".join(
                            "{}-{}: {}dB".format(
                                sat.constellation, sat.satellite_id, sat.snr
                            )
                            for sat in nav.satellite_details
                        )
                    )
                timestamp = metadata[0].instant_scan
            else:
                timestamp = None
            self.__add_kml_point(
                name,
                self._folder_gnss,
                coordinates,
                self._style_green,
                extended_data,
                timestamp=timestamp,
            )
        elif localization_type == self.SCAN_TYPE_REFERENCE_COORDINATES:
            extended_data = ET.Element("ExtendedData")
            ET.SubElement(extended_data, "Data", name="date").text = "{}".format(
                metadata
            )
            self.__add_kml_point(
                name,
                self._folder_reference,
                coordinates,
                self._style_black,
                extended_data,
            )
        else:
            raise NotImplementedError(
                "Type '{}' is not supported'".format(localization_type)
            )

    def save(self):
        tree = ET.ElementTree(self._kml)
        tree.write(self._filename, xml_declaration=True, encoding="utf-8")


## Code below is commented out but keep here for example purpose
# if __name__ == "__main__":
#     kml_output = kmlOutput("LR1110 test name", "./test.kml")

#     with open("./temp.log", "r") as file_input:
#         data = file_input.readlines()
#         for scan in data:
#             name = scan[23:26]
#             localisation_type = scan[29]
#             coordinates = Coordinate.from_string(scan[32:][:-1])
#             date = scan[1:20]
#             if localisation_type == "2":
#                 kml_output.add_point(
#                     name,
#                     "Wi-Fi",
#                     coordinates,
#                     [
#                         ScannedMacAddress(
#                             "00:11:22:33:44:55:66:77",
#                             WifiChannel("CHANNEL_3", 2400000),
#                             "TYPE_B",
#                             -20,
#                             0,
#                             0,
#                             0,
#                             0,
#                             date,
#                         ),
#                         ScannedMacAddress(
#                             "AA:11:22:33:44:55:66:77",
#                             WifiChannel("CHANNEL_3", 2500000),
#                             "TYPE_G",
#                             -30,
#                             0,
#                             0,
#                             0,
#                             0,
#                             date,
#                         ),
#                     ],
#                 )
#             else:
#                 nav_message_raw = "0101070808e210a8ef20e22e0014529bec007d0080a4522becb70e20513d41e5e9dd45b05a2115da3b8b67957d73c8f498caaa216e9aec23"
#                 kml_output.add_point(name, "GNSS", coordinates, [date, nav_message_raw])

#     kml_output.save()
