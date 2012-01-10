import time
from threading import Thread
import _pyxivoclient as xivoclient

class PyXiVOClientThread(Thread):
    def __init__(self, args = ''):
        Thread.__init__(self)
        self.xc_args = args
        self.xc_instance = xivoclient.new_PyXiVOClient()

    def instance(self):
        return self.xc_instance

    def run(self):
        xivoclient.PyXiVOClient_launch(self.xc_instance, self.xc_args)
        print "thread sleeping"
        time.sleep(1)
        print "thread woke up"

    def stop(self):
        xivoclient.PyXiVOClient_exit(self.xc_instance)
        self.join()
        xivoclient.delete_PyXiVOClient(self.xc_instance)

def pythread_version1(xc_args):
    xc_thread = PyXiVOClientThread(xc_args)
    xc_thread.start()
    # xc_instance = xc_thread.instance()

    print "sleeping"
    time.sleep(5)
    print "woke up"

    xc_thread.stop()

def pythread_version2(xc_args):
    xc_instance = xivoclient.new_PyXiVOClient()
    xc_thread = Thread(target = xivoclient.PyXiVOClient_launch, args = (xc_instance, xc_args))
    xc_thread.start()
    print "thread sleeping"
    time.sleep(10)

def cthread_version(xc_args):
    xc_instance = xivoclient.new_PyXiVOClient()
    xivoclient.PyXiVOClient_launch(xc_instance, xc_args)

    time.sleep(5)

    xivoclient.PyXiVOClient_exit(xc_instance)
    xivoclient.delete_PyXiVOClient(xc_instance)

# pythread_version1('client')
# pythread_version2('client')
cthread_version('client')
