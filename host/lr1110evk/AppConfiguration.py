from .BaseTypes import Coordinate
from .FieldTestPost.Core.GeoLocServiceClientBase import (
    GeoLocServiceClientGnss,
    GeoLocServiceClientWifi,
)
from datetime import datetime


class AppConfigurationException(Exception):
    pass


class WrongDateFormat(AppConfigurationException):
    def __init__(self, date_provided, date_format):
        self.date_provided = date_provided
        self.date_format = date_format

    def __str__(self):
        return "Error while parsing date: provided value: '{}', expected format: '{}'".format(
            self.date_provided, self.date_format
        )


class AppConfigurationBase:
    def __init__(self):
        self.gnss_server = None
        self.wifi_server = None
        self.dry_run = False
        self.verbosity = False
        self.approximate_gnss_server_localization = None

    def _set_from_arguments(self, args):
        self.gnss_server = GeoLocServiceClientGnss.from_token_and_url_info(
            args.glsAuthenticationToken,
            args.gnss_server_base_url,
            args.gnss_server_port,
            GeoLocServiceClientGnss.get_default_url_version(),
            GeoLocServiceClientGnss.get_default_url_path(),
        )
        self.wifi_server = GeoLocServiceClientWifi.from_token_and_url_info(
            args.glsAuthenticationToken,
            args.wifi_server_base_url,
            args.wifi_server_port,
            GeoLocServiceClientWifi.get_default_url_version(),
            GeoLocServiceClientWifi.get_default_url_path(),
        )
        self.dry_run = args.dry_run
        self.verbosity = args.verbose
        self.approximate_gnss_server_localization = Coordinate.from_string(
            args.approximateGnssServerLocalization
        )

    @classmethod
    def from_arg_parser(cls, args):
        configuration = cls()
        configuration._set_from_arguments(args)
        return configuration


class DemoAppConfiguration(AppConfigurationBase):
    PARSE_DATE_FORMAT = "%Y/%m/%d-%H:%M:%S"

    def __init__(self):
        self.fake_date = None
        self.device_address = None
        self.device_baud = None
        self.actual_coordinate = None
        self.approximate_gnss_lr1110_localization = None
        self.request_reverse_geo_coding = False
        self.fake_date = None

    def _set_from_arguments(self, args):
        super()._set_from_arguments(args)
        self.device_address = args.device_address
        self.device_baud = args.device_baud
        self.actual_coordinate = Coordinate.from_string(args.exactCoordinate)
        if args.gnss_assisted_scan_approximate_localization:
            self.approximate_gnss_lr1110_localization = Coordinate.from_string(
                args.gnss_assisted_scan_approximate_localization
            )
        else:
            self.approximate_gnss_lr1110_localization = (
                self.approximate_gnss_server_localization
            )
        self.request_reverse_geo_coding = args.reverse_geo_coding
        if args.fake_date:
            try:
                self.fake_date = datetime.strptime(
                    args.fake_date, DemoAppConfiguration.PARSE_DATE_FORMAT
                )
            except ValueError:
                raise WrongDateFormat(
                    args.fake_date, DemoAppConfiguration.PARSE_DATE_FORMAT
                )
        else:
            self.fake_date = None


class FieldTestConfiguration(AppConfigurationBase):
    def __init__(self):
        self.deactivate_wifi_requests = None
        self.user_defined_coordinate = None

    def _set_from_arguments(self, args):
        super()._set_from_arguments(args)
        self.deactivate_wifi_requests = args.no_wifi
        if args.user_defined_location:
            self.user_defined_coordinate = Coordinate.from_string(
                args.user_defined_location
            )
        else:
            self.user_defined_coordinate = None
