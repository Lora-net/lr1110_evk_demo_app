from .satelliteInfo import SatelliteInfo


class SatelliteResults:
    def __init__(
        self,
        satellite_id,
        status_info,
        timestamp1,
        timestamp2,
        doppler,
        bit_change_info1,
        bit_change_info2,
    ):
        self.__satellite_id = satellite_id
        self.__status_info = status_info
        self.__timestamp1 = timestamp1
        self.__timestamp2 = timestamp2
        self.__doppler = doppler
        self.__bit_change_info1 = bit_change_info1
        self.__bit_change_info2 = bit_change_info2

    @property
    def satellite_id(self):
        return self.__satellite_id

    @property
    def satellite_name(self):
        return SatelliteResults.satellite_id_to_name(self.__satellite_id)

    @property
    def status_info(self):
        return self.__status_info

    @property
    def timestamp1(self):
        return self.__timestamp1

    @property
    def timestamp2(self):
        return self.__timestamp2

    @property
    def doppler(self):
        return self.__doppler

    @property
    def bit_change_info1(self):
        return self.__bit_change_info1

    @property
    def bit_change_info2(self):
        return self.__bit_change_info2

    @staticmethod
    def from_nav_consumer(nav_consumer):
        satellite_id_raw = int.from_bytes(
            nav_consumer.consume_field(7), byteorder="little"
        )
        status_info = SatelliteInfo.from_nav_parser(nav_consumer)

        if status_info.timestamp1_exists:
            timestamp1_raw = nav_consumer.consume_field(19)
            timestamp1 = int.from_bytes(timestamp1_raw, byteorder="little")
        else:
            timestamp1 = None
        if status_info.timestamp2_exists:
            timestamp2_raw = nav_consumer.consume_field(19)
            timestamp2 = int.from_bytes(timestamp2_raw, byteorder="little")
        else:
            timestamp2 = None
        if status_info.doppler_exists:
            doppler_raw = nav_consumer.consume_field(15)
            doppler_value = SatelliteResults._doppler_value_from_raw_val(doppler_raw)
        else:
            doppler_value = None
        if status_info.bit_change_info1_exists:
            bit_change_info1_raw = nav_consumer.consume_field(8)
        else:
            bit_change_info1_raw = None
        if status_info.bit_change_info2_exists:
            bit_change_info2_raw = nav_consumer.consume_field(8)
        else:
            bit_change_info2_raw = None

        return SatelliteResults(
            satellite_id=satellite_id_raw,
            status_info=status_info,
            timestamp1=timestamp1,
            timestamp2=timestamp2,
            doppler=doppler_value,
            bit_change_info1=bit_change_info1_raw,
            bit_change_info2=bit_change_info2_raw,
        )

    @staticmethod
    def _doppler_value_from_raw_val(raw_value: bytes):
        doppler_value = int.from_bytes(raw_value, byteorder='little')
        if doppler_value & 0x4000:
            doppler_value |= 0xC000
            doppler_value = int.from_bytes(
                doppler_value.to_bytes(
                    length=2,
                    byteorder='little'
                ),
                byteorder='little',
                signed=True
            )
        return doppler_value

    @staticmethod
    def satellite_id_to_name(satellite_id: int):
        if 0 <= satellite_id <= 31:
            satellite_number = satellite_id + 1
            return f"GPS #{satellite_number}"
        elif 64 <= satellite_id <= 100:
            satellite_number = satellite_id - 63
            return f"BeiDou #{satellite_number}"
        elif 32 <= satellite_id <= 50:
            special_sbas_names = {
                34: "AUS-NZ",
                35: "EGNOS",
                38: "EGNOS",
                39: "GAGAN",
                40: "GAGAN",
                41: "MSAS",
                42: "ARTEMIS",
                43: "WAAS",
                44: "GAGAN",
                45: "WAAS",
                48: "EGNOS",
                49: "MSAS",
                50: "WAAS",
            }
            satellite_number = satellite_id + 88
            return f"SBAS #{satellite_number}"
        else:
            return f"RFU (sv_id: {satellite_id})"

    def __str__(self):
        return f"Sv Id: {self.satellite_id} (C/N: {self.status_info.c_n()}): timestamps {self.timestamp1}/{self.timestamp2}"
