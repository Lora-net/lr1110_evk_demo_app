from json import loads
from collections import namedtuple
from lr1110evk.BaseTypes import Coordinate
from lr1110evk.Job.KmlExport import kmlOutput


class KmlGeneratorException(Exception):
    pass


class KmlGeneratorUnknownTypeBaseException(KmlGeneratorException):
    def __init__(self, type_label, unknown_type, possible_types):
        self.unknown_type = unknown_type
        self.possible_types = possible_types
        self.type_label = type_label

    def __str__(self):
        return "Unknown {} type: '{}'. Possble line types are: {}".format(
            self.type_label, self.unknown_type, ", ".join(self.possible_types)
        )


class KmlGeneratorUnknownLineTypeException(KmlGeneratorUnknownTypeBaseException):
    def __init__(self, unknown_type, possible_types):
        super().__init__("line", unknown_type, possible_types)


class KmlGeneratorUnknownLocationTypeException(KmlGeneratorUnknownTypeBaseException):
    def __init__(self, unknown_type, possible_types):
        super().__init__("result", unknown_type, possible_types)


class KmlGeneratorInterpretLineException(KmlGeneratorException):
    def __init__(self, line):
        self.line = line

    def __str__(self):
        return "Failed to interpret line: '{}'".format(self.line)


KmlOutputPoint = namedtuple(
    "KmlOutputPoint", ["date_str", "kml_output_type", "coordinate", "metadata"]
)


class KmlGenerator:
    def __init__(self, input_filename, kml_output_file):
        self.LINE_TYPE_INTERPRETER = {"AFTER_DAS_LOC": KmlGenerator.interpret_das_loc}
        self.POSITION_TYPE_INTERPRETER = {
            "wifi": KmlGenerator.interpret_wifi_loc,
            "gnss": KmlGenerator.interpret_gnss_loc,
        }
        self.input_filename = input_filename
        self.kml = kmlOutput(name=None, filename=kml_output_file)
        self.failed_interpret_line_counter = 0
        self.success_interpret_line_counter = 0

    def interpret_file(self):
        kml_points = list()
        for line in open(self.input_filename, "r"):
            try:
                kml_points.extend(self.interpret_line(loads(line)))
            except KmlGeneratorInterpretLineException:
                self.failed_interpret_line_counter += 1
            self.success_interpret_line_counter += 1

        for kml_point in kml_points:
            self.kml.add_point(
                kml_point.date_str,
                kml_point.kml_output_type,
                kml_point.coordinate,
                kml_point.metadata,
            )
        self.kml.save()

    def interpret_line(self, line):
        line_type = line["record"]
        return self.interpret_line_type(line, line_type)

    def interpret_line_type(self, line, line_type):
        try:
            line_interpreter_from_type = self.LINE_TYPE_INTERPRETER[line_type]
        except KeyError:
            raise KmlGeneratorUnknownLineTypeException(
                line_type, self.LINE_TYPE_INTERPRETER.keys()
            )
        try:
            result = line_interpreter_from_type(self, line)
        except KeyError:
            raise KmlGeneratorInterpretLineException(line)
        return result

    def is_allowed_eui(self, eui):
        return True

    def interpret_das_loc(self, line):
        das_result_response = loads(line["das"])["result"]
        date_str = line["uplink"]["rx_timestamp"]
        kml_points = list()
        for eui, das_result_per_eui in das_result_response.items():
            if self.is_allowed_eui(eui):
                position_solution = das_result_per_eui["result"]["position_solution"]

                if "algorithm_type" in position_solution:
                    solution_type = position_solution["algorithm_type"]
                elif "ecef" in position_solution:
                    solution_type = "gnss"

                coordinate, metadata = self.interpret_location_type(
                    position_solution, solution_type
                )
                kml_output_type = self.get_kml_output_type_from_location_type(
                    solution_type
                )
                new_kml_point = KmlOutputPoint(
                    date_str, kml_output_type, coordinate, metadata
                )
                kml_points.append(new_kml_point)
        return kml_points

    @staticmethod
    def get_kml_output_type_from_location_type(location_type):
        location_type_mapper = {
            "wifi": kmlOutput.SCAN_TYPE_WIFI,
            "gnss": kmlOutput.SCAN_TYPE_GNSS,
        }
        try:
            kml_output_type = location_type_mapper[location_type]
        except KeyError:
            raise KmlGeneratorUnknownLocationTypeException(
                location_type, location_type_mapper.keys()
            )
        return kml_output_type

    def interpret_location_type(self, location_structure, location_type):
        try:
            location_interpreter_from_type = self.POSITION_TYPE_INTERPRETER[
                location_type
            ]
        except KeyError:
            raise KmlGeneratorUnknownLocationTypeException(
                location_type, self.POSITION_TYPE_INTERPRETER.keys()
            )
        coordinate, metadata = location_interpreter_from_type(self, location_structure)
        return coordinate, metadata

    def interpret_wifi_loc(self, location_result_structure):
        coordinate_array = location_result_structure["llh"]
        wifi_coordinate = Coordinate(
            latitude=coordinate_array[0],
            longitude=coordinate_array[1],
            altitude=coordinate_array[2],
        )
        metadata = None
        return wifi_coordinate, metadata

    def interpret_gnss_loc(self, location_result_structure):
        coordinate_array = location_result_structure["llh"]
        gnss_coordinate = Coordinate(
            latitude=coordinate_array[0],
            longitude=coordinate_array[1],
            altitude=coordinate_array[2],
        )
        metadata = None
        return gnss_coordinate, metadata
