from math import ceil


class NavMessageConsumerException(Exception):
    pass


class NavMessageConsumerBitsExtractionOutOfBound(NavMessageConsumerException):
    def __init__(self, payload_length, bit_position, n_bits):
        self.payload_length = payload_length
        self.bit_position = bit_position
        self.n_bits = n_bits

    def __str__(self):
        return "Bit extraction out of bound. Payload has {} bits, trying to extract up to bit {}".format(
            self.payload_length * 8, self.bit_position + self.n_bits
        )


class NavMessageConsumer:
    def __init__(self, nav_message_raw):
        self.__nav_message_raw = nav_message_raw
        self.bit_location = 0

    @property
    def nav_message_raw(self):
        return self.__nav_message_raw

    @property
    def remaining_bits(self):
        return (len(self.__nav_message_raw) * 8) - self.bit_location

    @staticmethod
    def extract_bits_from_payload(payload, bit_position, n_bits):
        """ Extract n_bits starting from bit_position in payload"""

        # 0. Check one is not trying to get bits out of bounds
        if (bit_position + n_bits) > (len(payload) * 8):
            raise NavMessageConsumerBitsExtractionOutOfBound(
                len(payload), bit_position, n_bits
            )

        # 1. Convert to integer
        payload_integer = int.from_bytes(payload, byteorder="little")

        # 2. Compute mask, shift and length of the result
        mask = (2 ** (bit_position + n_bits) - 1) - (2 ** (bit_position) - 1)
        shift = bit_position
        result_length = ceil(n_bits / 8)

        # 3. Apply the extraction
        payload_extracted_integer = (payload_integer & mask) >> shift

        # 4. Convert back to byte
        payload_extracted = payload_extracted_integer.to_bytes(
            length=result_length, byteorder="little"
        )

        return payload_extracted

    def consume_field(self, n_bits_field):
        field_value = NavMessageConsumer.extract_bits_from_payload(
            self.__nav_message_raw, self.bit_location, n_bits_field
        )
        self.bit_location += n_bits_field
        return field_value
