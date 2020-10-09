from .baseTypes import (
    GnssSolverDestination,
    GnssSolverFrameTypeNav,
    HostDestination,
    GnssSolverDestination,
    GnssSolverNavMessage,
    HostNavMessage,
)
from .core import NavMessageConsumer, GetterFromId, ConstellationResults
from collections import namedtuple

Coordinate = namedtuple("Coordinate", ["latitude", "longitude", "altitude"])


class NavParser:
    def __init__(self):
        pass

    @classmethod
    def get_builder_from_destination(cls, destination):
        destination_to_builder_mapper = {
            HostDestination: cls.build_host_nav,
            GnssSolverDestination: cls.build_gnss_solver_nav,
        }
        return destination_to_builder_mapper[destination]

    @classmethod
    def extract_destination_id(cls, nav_message):
        destination_id_raw = NavMessageConsumer.extract_bits_from_payload(
            nav_message, 0, 8
        )
        return GetterFromId.get_destination_from_id(destination_id_raw)

    @classmethod
    def extract_gnss_solver_frame_type(cls, nav_message):
        gnss_frame_type_raw = NavMessageConsumer.extract_bits_from_payload(
            nav_message, 8, 4
        )
        return GetterFromId.get_gnss_solver_frame_type_from_id(gnss_frame_type_raw)

    @classmethod
    def build_host_nav(cls, nav_consumer):
        nav_host_status = GetterFromId.get_host_status_from_id(
            nav_consumer.consume_field(8)
        )
        nav_message = HostNavMessage(nav_host_status)
        return nav_message

    @classmethod
    def build_gnss_solver_nav(cls, nav_consumer):
        frame_type = GetterFromId.get_gnss_solver_frame_type_from_id(
            nav_consumer.consume_field(8)
        )
        gps_time_indicator = int.from_bytes(
            nav_consumer.consume_field(16), byteorder="little"
        )
        has_assistance_position = (
            True if nav_consumer.consume_field(1) == b"\x01" else False
        )
        if has_assistance_position:
            latitude = (
                int.from_bytes(
                    nav_consumer.consume_field(12), byteorder="little", signed=True
                )
                * 90
                / 2048
            )
            longitude = (
                int.from_bytes(
                    nav_consumer.consume_field(12), byteorder="little", signed=True
                )
                * 180
                / 2048
            )

            assistance_position = Coordinate(latitude, longitude, 0)
        else:
            assistance_position = None

        constellations = list()
        while True:
            new_constellation = ConstellationResults.from_nav_consumer(nav_consumer)
            constellations.append(new_constellation)
            # There may be remaining bits that may be padding.
            # In the case of NAV message, a constellation must have at least 8 bits
            # to hold the Modulation Type and the Number of satellites.
            # If the number remaining bits is less than 8, then it means it is padding bits.
            if nav_consumer.remaining_bits < 8:
                break
        nav_message = GnssSolverNavMessage(
            assistance_position=assistance_position,
            frame_type=frame_type,
            constellation_results=constellations,
            gps_time_indicator=gps_time_indicator,
        )
        return nav_message

    @classmethod
    def parse(cls, nav_message):
        nav_consumer = NavMessageConsumer(nav_message)
        destination = GetterFromId.get_destination_from_id(
            nav_consumer.consume_field(8)
        )
        builder = cls.get_builder_from_destination(destination)
        nav_message = builder(nav_consumer)
        return nav_message
