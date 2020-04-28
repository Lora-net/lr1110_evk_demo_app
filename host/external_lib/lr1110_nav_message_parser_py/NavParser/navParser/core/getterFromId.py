from ..baseTypes import (
    HostDestination,
    GnssSolverDestination,
    GnssDmcDestination,
    GnssSolverFrameTypeNoAssistance,
    GnssSolverFrameTypeNav,
    HostStatusBaseOk,
    HostStatusCommandUnexpected,
    HostStatusCommandNotImplemented,
    HostStatusCommandParametersInvalid,
    HostStatusMessageSanityCheckError,
    HostStatusIqCaptureFail,
    HostStatusNoTime,
    HostStatusNoSatelliteDetected,
    HostStatusAlmanacTooOld,
    HostStatusAlmanacUpdateCrcError,
    HostStatusAlmanacUpdateFlashError,
    HostStatusAlmanacUpdateTooOldError,
    CnRangeVeryHigh,
    CnRangeHigh,
    CnRangeLow,
    CnRangeVeryLow,
    NavParserDestinationIdUnknownException,
    NavParserGnssSolverFrameTypeUnknownException,
    NavParserHostStatusUnknownException,
    NavParserCnRangeUnknownException,
    NavParserModulationTypeUnknownException,
    GpsModulationType,
    BeidouModulationType,
)


def id_to_class_mapper(class_list):
    return {cl.get_id(): cl for cl in class_list}


class GetterFromId:
    ALL_DESTINATIONS = id_to_class_mapper(
        [HostDestination, GnssSolverDestination, GnssDmcDestination]
    )
    ALL_GNSS_SOLVER_FRAME_TYPES = id_to_class_mapper(
        [GnssSolverFrameTypeNoAssistance, GnssSolverFrameTypeNav]
    )
    ALL_HOST_STATUS = id_to_class_mapper(
        [
            HostStatusBaseOk,
            HostStatusCommandUnexpected,
            HostStatusCommandNotImplemented,
            HostStatusCommandParametersInvalid,
            HostStatusMessageSanityCheckError,
            HostStatusIqCaptureFail,
            HostStatusNoTime,
            HostStatusNoSatelliteDetected,
            HostStatusAlmanacTooOld,
            HostStatusAlmanacUpdateCrcError,
            HostStatusAlmanacUpdateFlashError,
            HostStatusAlmanacUpdateTooOldError,
        ]
    )
    ALL_CN_RANGES = id_to_class_mapper(
        [CnRangeVeryHigh, CnRangeHigh, CnRangeLow, CnRangeVeryLow]
    )
    ALL_MODULATION_TYPES = id_to_class_mapper([GpsModulationType, BeidouModulationType])

    @staticmethod
    def __get_from_mapper_or_raise(mapper, element, not_in_mapper_exception):
        try:
            return mapper[element]
        except KeyError:
            raise not_in_mapper_exception(element, mapper)

    @classmethod
    def get_destination_from_id(cls, raw_id):
        return cls.__get_from_mapper_or_raise(
            cls.ALL_DESTINATIONS, raw_id, NavParserDestinationIdUnknownException
        )

    @classmethod
    def get_gnss_solver_frame_type_from_id(cls, raw_id):
        return cls.__get_from_mapper_or_raise(
            cls.ALL_GNSS_SOLVER_FRAME_TYPES,
            raw_id,
            NavParserGnssSolverFrameTypeUnknownException,
        )

    @classmethod
    def get_host_status_from_id(cls, raw_id):
        return cls.__get_from_mapper_or_raise(
            cls.ALL_HOST_STATUS, raw_id, NavParserHostStatusUnknownException
        )

    @classmethod
    def get_cn_range_from_id(cls, raw_id):
        return cls.__get_from_mapper_or_raise(
            cls.ALL_CN_RANGES, raw_id, NavParserCnRangeUnknownException
        )

    @classmethod
    def get_modulation_type_from_id(cls, raw_id):
        return cls.__get_from_mapper_or_raise(
            cls.ALL_MODULATION_TYPES, raw_id, NavParserModulationTypeUnknownException
        )
