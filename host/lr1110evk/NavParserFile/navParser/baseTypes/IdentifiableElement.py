class IdentifiedElements:
    _id = None
    _name = None

    @classmethod
    def get_id(cls):
        return cls._id

    @classmethod
    def get_name(cls):
        return cls._name


class DestinationBase(IdentifiedElements):
    pass


class HostDestination(DestinationBase):
    _id = b"\x00"
    _name = "Host"


class GnssSolverDestination(DestinationBase):
    _id = b"\x01"
    _name = "GNSS solver"


class GnssSolverFrameTypeBase(IdentifiedElements):
    def __str__(self):
        return "{}".format(self.get_name())


class GnssSolverFrameTypeNoAssistance(GnssSolverFrameTypeBase):
    _id = b"\x00"
    _name = "No assistance position"


class GnssSolverFrameTypeNav(GnssSolverFrameTypeBase):
    _id = b"\x01"
    _name = "NAV message"


class HostStatusBase(IdentifiedElements):
    pass


class HostStatusBaseOk(HostStatusBase):
    _id = b"\x00"
    _name = "Ok"


class HostStatusCommandUnexpected(HostStatusBase):
    _id = b"\x01"
    _name = "Command unexpected"


class HostStatusCommandNotImplemented(HostStatusBase):
    _id = b"\x02"
    _name = "Command not implemented"


class HostStatusCommandParametersInvalid(HostStatusBase):
    _id = b"\x03"
    _name = "Command parameters invalid"


class HostStatusMessageSanityCheckError(HostStatusBase):
    _id = b"\x04"
    _name = "Message sanity check error"


class HostStatusIqCaptureFail(HostStatusBase):
    _id = b"\x05"
    _name = "IQ capture fail"


class HostStatusNoTime(HostStatusBase):
    _id = b"\x06"
    _name = "No time"


class HostStatusNoSatelliteDetected(HostStatusBase):
    _id = b"\x07"
    _name = "No satellite detected"


class HostStatusAlmanacTooOld(HostStatusBase):
    _id = b"\x08"
    _name = "Almanac in flash too old"


class HostStatusAlmanacUpdateCrcError(HostStatusBase):
    _id = b"\x09"
    _name = "Almanac update fail due to CRC error"


class HostStatusAlmanacUpdateFlashError(HostStatusBase):
    _id = b"\x0A"
    _name = "Almanac update fail due to flash integrity error"


class HostStatusAlmanacUpdateTooOldError(HostStatusBase):
    _id = b"\x0B"
    _name = "Almanac update fail due to almanac date too old"


class CnRangeBase(IdentifiedElements):
    def __str__(self):
        return "{}".format(self.get_name())


class CnRangeVeryHigh(CnRangeBase):
    _id = b"\x00"
    _name = ">= 45"


class CnRangeHigh(CnRangeBase):
    _id = b"\x01"
    _name = "[41,45["


class CnRangeLow(CnRangeBase):
    _id = b"\x02"
    _name = "[37,41["


class CnRangeVeryLow(CnRangeBase):
    _id = b"\x03"
    _name = "< 37"


class ModulationTypeBase(IdentifiedElements):
    def __str__(self):
        return "{}".format(self.get_name())


class GpsModulationType(ModulationTypeBase):
    _id = b"\x01"
    _name = "GPS"


class BeidouModulationType(ModulationTypeBase):
    _id = b"\x02"
    _name = "BeiDou"
