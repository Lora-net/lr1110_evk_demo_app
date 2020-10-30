from lr1110evk.Tools import KmlGenerator
from argparse import ArgumentParser
import pkg_resources


def main():
    version = pkg_resources.get_distribution("lr1110evk").version
    parser = ArgumentParser(
        description="Tool to generate a KML file from Node-Red Application Server output file"
    )
    parser.add_argument(
        "serverFile",
        help="File containing the localisation solved from Application Server",
    )
    parser.add_argument("outputKmlFile", help="File to write")
    parser.add_argument(
        "--verbose", "-v", help="Verbose", action="store_true", default=False
    )
    parser.add_argument("--version", action="version", version=version)
    args = parser.parse_args()

    kml_generator = KmlGenerator(
        input_filename=args.serverFile, kml_output_file=args.outputKmlFile
    )
    kml_generator.interpret_file()

    print(
        "Number of line successfully interpreted: {}".format(
            kml_generator.success_interpret_line_counter
        )
    )
    print(
        "Number of line failed to be interpreted: {}".format(
            kml_generator.failed_interpret_line_counter
        )
    )
