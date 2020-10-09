from setuptools import setup, find_packages

PACKAGE_NAME = "lr1110evk"


setup(
    name=PACKAGE_NAME,
    version_format="{tag}.dev{commitcount}+{gitsha}",
    setup_requires=["setuptools-git-version"],
    include_package_data=True,
    packages=find_packages(),
    author="Semtech",
    install_requires=[
        "pyserial",
        "pyyaml",
        "requests",
        "jsl",
        "jsonschema",
        "pynmea2",
    ],
    description="Computer companion software for lr1110 EVK project",
    entry_points={
        "console_scripts": [
            "Lr1110FieldTest = lr1110evk.__main__:drive_field_tests",
            "FieldTestPost = lr1110evk.FieldTestPost.__main__:post_analyzis_fetch_results",
            "Lr1110Demo = lr1110evk.__main__:entry_point_demo",
            "NavParser = lr1110evk.NavParserFile.__main__:entry_point_nav_parser_file",
            "UsbConnectionCheck = lr1110evk.SerialExchange.SerialHandlerConnectionTest:entry_point_connection_tester",
            "AlmanacUpdate = lr1110evk.main_almanac_update:entry_point_update_almanac",
        ]
    },
)
