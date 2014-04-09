try:
    basestr
except NameError:
    basestring = str
    unicode = str
