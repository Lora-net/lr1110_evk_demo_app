from .satelliteResults import SatelliteResults
from .getterFromId import GetterFromId


class ConstellationResults:
    def __init__(self, modulation_type, satellites):
        self.__satellites = satellites
        self.__modulation_type = modulation_type

    @property
    def modulation_type(self):
        return self.__modulation_type

    @property
    def satellites(self):
        return self.__satellites

    @staticmethod
    def from_nav_consumer(nav_consumer):
        modulation_type = GetterFromId.get_modulation_type_from_id(
            nav_consumer.consume_field(4)
        )
        number_of_sv_raw = nav_consumer.consume_field(4)
        satellites = list()
        for _ in range(0, int.from_bytes(number_of_sv_raw, byteorder="little")):
            new_satellite = SatelliteResults.from_nav_consumer(nav_consumer)
            satellites.append(new_satellite)
        return ConstellationResults(
            modulation_type=modulation_type, satellites=satellites
        )

    def __str__(self):
        return "Modulation type: {}\n   - ".format(
            str(self.modulation_type())
        ) + "\n   - ".join([str(sat) for sat in self.satellites])
