from .IdentifiableElement import GnssSolverDestination, GnssSolverFrameTypeNav


class NavParserException(Exception):
    pass


class NavParserElementNotInMapperException(NavParserException):
    MAPPER_NAMING = None

    def __init__(self, element, mapper):
        self.mapper = mapper
        self.element = element

    def __str__(self):
        all_ids_str = ", ".join(
            [
                ": ".join([str(raw_id), name.get_name()])
                for raw_id, name in self.mapper.items()
            ]
        )
        return "{} is not a known {}. Known IDs are: {}".format(
            self.element, self.MAPPER_NAMING, all_ids_str
        )


class NavParserDestinationIdUnknownException(NavParserElementNotInMapperException):
    MAPPER_NAMING = "destination id"


class NavParserGnssSolverFrameTypeUnknownException(
    NavParserElementNotInMapperException
):
    MAPPER_NAMING = "frame type"


class NavParserHostStatusUnknownException(NavParserElementNotInMapperException):
    MAPPER_NAMING = "host status"


class NavParserCnRangeUnknownException(NavParserElementNotInMapperException):
    MAPPER_NAMING = "c/n range"


class NavParserModulationTypeUnknownException(NavParserElementNotInMapperException):
    MAPPER_NAMING = "modulation type"


class NotGnssSolverDestinationException(NavParserException):
    def __init__(self, actual_destination):
        self.actual_destination = actual_destination

    def __str__(self):
        return "The NAV message must be destinated to '{destination_name}' (id={destination_id!s}) but it is destinated to '{actual_destination_name}' (id={actual_destination_id!s})".format(
            destination_name=GnssSolverDestination.get_name(),
            destination_id=GnssSolverDestination.get_id(),
            actual_destination_name=self.actual_destination.get_name(),
            actual_destination_id=self.actual_destination.get_id(),
        )


class NotNavFrameTypeException(NavParserException):
    def __init__(self, actual_frame_type):
        self.actual_frame_type = actual_frame_type

    def __str__(self):
        return "The NAV message must have frame type set to '{frame_type_name}' (id={frame_type_id}) but is set to {actual_frame_type_name} (id={actual_frame_type_id})".format(
            frame_type_name=GnssSolverFrameTypeNav.get_name(),
            frame_type_id=GnssSolverFrameTypeNav.get_id(),
            actual_frame_type_name=self.actual_frame_type.get_name(),
            actual_frame_type_id=self.actual_frame_type.get_id(),
        )
