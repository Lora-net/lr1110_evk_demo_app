from ..baseTypes import CnRangeVeryHigh, CnRangeHigh, CnRangeLow, CnRangeVeryLow
from .getterFromId import GetterFromId


class SatelliteInfo:
    def __init__(
        self,
        timestamp1_exists,
        timestamp2_exists,
        doppler_exists,
        doppler_error,
        bit_change_info1,
        bit_change_info2,
        c_n,
    ):
        self.__timestamp1_exists = timestamp1_exists
        self.__timestamp2_exists = timestamp2_exists
        self.__doppler_exists = doppler_exists
        self.__doppler_error = doppler_error
        self.__bit_change_info1_exists = bit_change_info1
        self.__bit_change_info2_exists = bit_change_info2
        self.__c_n = c_n

    @property
    def timestamp1_exists(self):
        return self.__timestamp1_exists

    @property
    def timestamp2_exists(self):
        return self.__timestamp2_exists

    @property
    def doppler_exists(self):
        return self.__doppler_exists

    @property
    def doppler_error(self):
        return self.__doppler_error

    @property
    def bit_change_info1_exists(self):
        return self.__bit_change_info1_exists

    @property
    def bit_change_info2_exists(self):
        return self.__bit_change_info2_exists

    @property
    def c_n(self):
        return self.__c_n

    @staticmethod
    def from_nav_parser(nav_parser):
        c_n = GetterFromId.get_cn_range_from_id(nav_parser.consume_field(2))
        bit_change_info2_exists = bool(nav_parser.consume_field(1) == b"\x01")
        bit_change_info1_exists = bool(nav_parser.consume_field(1) == b"\x01")
        doppler_error = bool(nav_parser.consume_field(1) == b"\x01")
        doppler_exists = bool(nav_parser.consume_field(1) == b"\x01")
        timestamp2_exists = bool(nav_parser.consume_field(1) == b"\x01")
        timestamp1_exists = bool(nav_parser.consume_field(1) == b"\x01")
        return SatelliteInfo(
            timestamp1_exists,
            timestamp2_exists,
            doppler_exists,
            doppler_error,
            bit_change_info1_exists,
            bit_change_info2_exists,
            c_n,
        )
