from .IdentifiableElement import (
    HostDestination,
    GnssSolverDestination,
    GnssDmcDestination,
)


class NavMessageBase:
    DESTINATION = None

    @classmethod
    def __get_destination(cls):
        return cls.DESTINATION

    @property
    def destination(self):
        return self.__get_destination()


class HostNavMessage(NavMessageBase):
    DESTINATION = HostDestination

    def __init__(self, status):
        self.__status = status

    @property
    def status(self):
        return self.__status


class GnssSolverNavMessage(NavMessageBase):
    DESTINATION = GnssSolverDestination

    def __init__(
        self,
        assistance_position,
        frame_type,
        constellation_results,
        gps_time_indicator,
    ):
        self.__assistance_position = assistance_position
        self.__frame_type = frame_type
        self.__constellation_results = constellation_results
        self.__gps_time_indicator = gps_time_indicator

    @property
    def assistance_position(self):
        return self.__assistance_position

    @property
    def frame_type(self):
        return self.__frame_type

    @property
    def constellation_results(self):
        return self.__constellation_results

    @property
    def gps_time_indicator(self):
        return self.__gps_time_indicator

    def __str__(self):
        return (
            f"- Assistance Position: {self.assistance_position}\n- GPS time indicator: {self.gps_time_indicator}\n- frame type: '{self.frame_type()}'"
            + "\n- Constellations:\n - {}".format(
                "\n - ".join(
                    [str(constellation) for constellation in self.constellation_results]
                )
            )
        )


class GnssDmcNavMessage(NavMessageBase):
    DESTINATION = GnssDmcDestination
