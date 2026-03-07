import ROOT as rt

class RootIterator:
    def __init__(self, o):
        # Prefer ROOT6 python iteration protocol to avoid deprecated
        # createIterator() calls (ROOT >= 6.32 warns about it).
        self._py_it = None
        self._it = None
        self._seq_obj = None
        self._seq_idx = 0
        self._seq_size = 0

        try:
            self._py_it = iter(o)
            return
        except TypeError:
            pass

        # RooAbsCollection supports getSize()/at(i), which avoids createIterator().
        if hasattr(o, "getSize") and hasattr(o, "at"):
            self._seq_obj = o
            self._seq_size = int(o.getSize())
            return

        if hasattr(o, "Class") and o.Class().InheritsFrom("TIterator"):
            self._it = o
        elif hasattr(o, "MakeIterator"):
            self._it = o.MakeIterator()
        elif hasattr(o, "componentIterator"):
            self._it = o.componentIterator()
        elif hasattr(o, "createIterator"):
            # Backward-compatible fallback for very old ROOT classes.
            self._it = o.createIterator()
        else:
            raise TypeError(
                "Object of type %s does not provide a ROOT iterator" % type(o).__name__
            )

    def __iter__(self):
        return self

    def __next__(self):
        if self._py_it is not None:
            return next(self._py_it)
        if self._seq_obj is not None:
            while self._seq_idx < self._seq_size:
                obj = self._seq_obj.at(self._seq_idx)
                self._seq_idx += 1
                if obj:
                    return obj
            raise StopIteration
        n = self._it.Next()
        if not n:  # None or null pointer
            raise StopIteration
        return n

    next = __next__

