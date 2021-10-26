from .RequestBase import (
    RequestGnssGls,
    RequestGnssMultiFrameGls,
)
from lr1110evk.BaseTypes import (
    GroupingMultiFrameGnss,
    MultiFrameGnssBase,
    SingleFrameGnss,
    SlidingMultiFrameGnss,
    ScannedGnss,
)


class GnssSolvingStrategyBase:
    def get_container(self) -> MultiFrameGnssBase:
        raise NotImplementedError

    def build_request(self):
        raise NotImplementedError


class GnssSingleSolvingStrategy(GnssSolvingStrategyBase):
    def __init__(self):
        super().__init__()
        self.container = SingleFrameGnss()

    def get_container(self):
        return self.container

    def build_request(self):
        scanned_message = self.container.get_navs()[0]
        return RequestGnssGls(
            payload=scanned_message.nav_message,
            timestamp=scanned_message.instant_scan,
        )


class GnssMultiFrameStrategyBase(GnssSolvingStrategyBase):
    def build_request(self):
        nav_messages = self.container.get_navs()
        return RequestGnssMultiFrameGls(nav_messages=nav_messages)


class GnssMultiFrameSlidingStrategy(GnssMultiFrameStrategyBase):
    def __init__(self, depth):
        super().__init__()
        self.container = SlidingMultiFrameGnss(depth=depth)

    def get_container(self):
        return self.container


class GnssMultiFrameGroupingStrategy(GnssMultiFrameStrategyBase):
    def __init__(self, max_length):
        super().__init__()
        self.container = GroupingMultiFrameGnss(max_length=max_length)

    def get_container(self):
        return self.container
