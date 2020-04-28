from setuptools import setup, find_packages

PACKAGE_NAME = 'NavParser'

setup(
    name=PACKAGE_NAME,
    version_format='{tag}.dev{commitcount}+{gitsha}',
    setup_requires=[
        'setuptools-git-version'
    ],
    packages=find_packages(),
    author="Semtech",
    description="Library for parsing NAV messages",
    entry_points={
        'console_scripts': [
            'NavParser = NavParser.__main__:entry_point_nav_parser_file',
        ]
    }
)
