from .navParser import NavParser
from .baseTypes import GnssSolverDestination


class NavMessageParserException(Exception):
    pass


class NotGnssSolverDestinationException(NavMessageParserException):
    def __init__(self, destination):
        self.actual_destination = destination

    def __str__(self):
        return "The NAV message must be destinated to GNSS solver, but its actual destination is {}".format(
            self.actual_destination.get_name()
        )


class NavMessageParser:
    def __init__(self):
        pass

    @staticmethod
    def parse(nav_message):
        nav_message_raw = NavMessageParser.nav_string_to_bytes(nav_message)
        satellites_description = NavMessageParser._parse_nav_message(nav_message_raw)
        return satellites_description

    @staticmethod
    def _parse_nav_message(nav_message):
        parsed_nav_message = NavParser.parse(nav_message)
        return parsed_nav_message

    @staticmethod
    def nav_string_to_bytes(nav_string):
        return bytes.fromhex(nav_string)

    @staticmethod
    def get_cn_vs_from_parsed_message(parsed_nav_message):
        cn_vs_satellite_id = dict()
        if parsed_nav_message.destination != GnssSolverDestination:
            raise NotGnssSolverDestinationException(parsed_nav_message.destination)
        for constellation in parsed_nav_message.constellation_results:
            for satellite in constellation.satellites:
                satellite_name = satellite.satellite_name
                c_n = satellite.c_n.get_name()
                cn_vs_satellite_id[satellite_name] = c_n
        return cn_vs_satellite_id
