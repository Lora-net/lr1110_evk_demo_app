from .cnGetterFromNav import CNGetterFromNav


class SatelliteResults:
    def __init__(self, satellite_id, c_n):
        self.__satellite_id = satellite_id
        self.__c_n = c_n

    @property
    def satellite_id(self):
        return self.__satellite_id

    @property
    def satellite_name(self):
        return SatelliteResults.satellite_id_to_name(self.__satellite_id)

    @property
    def c_n(self):
        return self.__c_n

    @staticmethod
    def from_nav_consumer(nav_consumer):
        satellite_id_raw = int.from_bytes(
            nav_consumer.consume_field(7), byteorder="little"
        )
        c_n = CNGetterFromNav.get_c_n_from_nav(nav_consumer)

        return SatelliteResults(satellite_id=satellite_id_raw, c_n=c_n)

    @staticmethod
    def satellite_id_to_name(satellite_id: int):
        if 0 <= satellite_id <= 31:
            satellite_number = satellite_id + 1
            return "GPS #{}".format(satellite_number)
        elif 64 <= satellite_id <= 100:
            satellite_number = satellite_id - 63
            return "BeiDou #{}".format(satellite_number)
        elif 32 <= satellite_id <= 50:
            satellite_number = satellite_id + 88
            return "SBAS #{}".format(satellite_number)
        else:
            return "RFU (sv_id: {})".format(satellite_id)

    def __str__(self):
        return "Sv Id: {} - C/N: {}".format(self.satellite_id, self.c_n())
