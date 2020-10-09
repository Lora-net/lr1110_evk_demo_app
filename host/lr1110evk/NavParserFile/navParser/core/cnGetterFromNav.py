from .getterFromId import GetterFromId


class CNGetterFromNav:
    @staticmethod
    def get_c_n_from_nav(nav_parser):
        c_n = GetterFromId.get_cn_range_from_id(nav_parser.consume_field(2))

        index = 0
        if bool(nav_parser.consume_field(1) == b"\x01"):
            index += 8
        if bool(nav_parser.consume_field(1) == b"\x01"):
            index += 8
        if bool(nav_parser.consume_field(1) == b"\x01"):
            index += 0
        if bool(nav_parser.consume_field(1) == b"\x01"):
            index += 15
        if bool(nav_parser.consume_field(1) == b"\x01"):
            index += 19
        if bool(nav_parser.consume_field(1) == b"\x01"):
            index += 19
        if bool(nav_parser.consume_field(index) == b"\x01"):
            index += 0
        return c_n
