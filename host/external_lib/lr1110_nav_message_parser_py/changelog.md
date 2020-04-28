# Python NAV Message Parser

## [0.4] 2020-03-23

### Changed

- Renaming to LR1110
- Determined the human readable value for constellation type instead of keeping raw value
- Sanitize printing of human readable NAV message
- The method `NavMessageParser.get_cn_vs_from_parsed_message` now returns the satellite names instead of the satellite IDs

### Fixed

- Handling of negative values when parsing satellite doppler field
- Fix IDs of GNSS and BeiDou modulation types

## [0.3] 2020-01-28

### Changed

- Add a getter for the gps time indicator member of GNSS solver NAV message

### Fixed

- Remove the version field of GNSS solver NAV message
- Change the parsing length of frame type of GNSS solver NAV message from 4 bits to 8 bits

## [0.2] 2020-01-27

### Changed

- Modify parser to be compliant with NAV parser with Sniff date included

### Remove

- Drop support for parsing NAV message without date included