from collections import deque


class MultiFrameException(Exception):
    pass


class GroupingMultiFrameNotEnoughNavException(MultiFrameException):
    def __init__(self, n_nav, n_nav_excepted):
        super().__init__()
        self.n_nav = n_nav
        self.n_nav_excepted = n_nav_excepted

    def __str__(self):
        return "Not enough NAV message available in block multi frame. Excepted {} but {} are available".format(
            self.n_nav_excepted, self.n_nav
        )


class MultiFrameGnssBase:
    def push_nav_message(self, nav):
        raise NotImplementedError

    def get_navs(self):
        raise NotImplementedError

    def flush(self):
        raise NotImplementedError


class SlidingMultiFrameGnss(MultiFrameGnssBase):
    """Holds a sliding list of nav messages limited to a certain amount"""

    def __init__(self, depth):
        self.fifo = deque(maxlen=depth)

    def push_nav_message(self, nav):
        self.fifo.append(nav)

    def get_navs(self):
        return [nav for nav in self.fifo]

    def flush(self):
        self.fifo.clear()

    def __str__(self):
        return "SlidingMultiFrame(depth={})".format(self.fifo.maxlen)


class GroupingMultiFrameGnss(MultiFrameGnssBase):
    """Holds a block of NAV and forbid access unless the max amount is reached. Reset the content when a new NAV is added"""

    def __init__(self, max_length):
        self.max_length = max_length
        self.container = list()

    def push_nav_message(self, nav):
        if len(self.container) < self.max_length:
            self.container.append(nav)
        else:
            self.container.clear()
            self.container.append(nav)

    def get_navs(self):
        n_nav = len(self.container)
        # Check how many NAV messages are available. If not enough have been stored yet, raise an exception
        if n_nav < self.max_length:
            raise GroupingMultiFrameNotEnoughNavException(n_nav, self.max_length)
        else:
            return self.container

    def flush(self):
        self.container.clear()

    def __str__(self):
        return "GroupingMultiFrame(depth={})".format(self.max_length)


class SingleFrameGnss(SlidingMultiFrameGnss):
    def __init__(self):
        super().__init__(depth=1)

    def __str__(self):
        return "SingleFrame"
