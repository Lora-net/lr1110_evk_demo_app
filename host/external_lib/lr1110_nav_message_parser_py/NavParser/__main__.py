from argparse import ArgumentParser
import pkg_resources
from NavParser.NavMessageParser import NavMessageParser
from NavParser.navParser.baseTypes import GnssSolverDestination


def entry_point_nav_parser_file():
    version = pkg_resources.get_distribution("NavParser").version
    parser = ArgumentParser(description="Parse NAV messages from LR1110")
    parser.add_argument("--version", action="version", version=version)
    parser.add_argument(
        "nav_message",
        help="NAV message to interpret",
    )
    args = parser.parse_args()
    parsed_nav = NavMessageParser.parse(args.nav_message)
    if parsed_nav.destination != GnssSolverDestination:
        print("The NAV message is not for the solver")
    print("Nav Message:\n{}".format(parsed_nav))


if __name__ == '__main__':
    entry_point_nav_parser_file()