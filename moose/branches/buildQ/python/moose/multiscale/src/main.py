import logging 
import debug
logger = logging.getLogger('multiscale')
try:
    from lxml import etree
    debug.printDebug("DEBUG", "running with lxml.etree")
except ImportError:
    try:
        # Python 2.5
        import xml.etree.cElementTree as etree
        debug.printDebug("DEBUG", "running with cElementTree")
    except ImportError:
        try:
            # Python 2.5
            import xml.etree.ElementTree as etree
            debug.printDebug("DEBUG", "running with ElementTree")
        except ImportError:
            try:
              # normal cElementTree install
              import cElementTree as etree
              debug.printDebug("DEBUG", "running with cElementTree")
            except ImportError:
                try:
                    # normal ElementTree install
                    import elementtree.ElementTree as etree
                    debug.printDebug("DEBUG", "running with ElementTree")
                except ImportError:
                    debug.prefix("FATAL", "Failed to import ElementTree")
                    os._exit(1)

print("Doing really nothing")
