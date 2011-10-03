# build projects contained in subdirectories
TEMPLATE = subdirs
SUBDIRS  = src/xlets/xletweb \
           src/xlets/videoxlet \
           src/xlets/xletnull \
           src/xlets/conference \
           src/xlets/history \
           src/xlets/switchboard \
           src/xlets/identity \
           src/xlets/datetime \
           src/xlets/features \
           src/xlets/queues \
           src/xlets/agents \
           src/xlets/agentsnext \
           src/xlets/agentdetails \
           src/xlets/queuedetails \
           src/xlets/queueentrydetails \
           src/xlets/operator \
           src/xlets/calls \
           src/xlets/records \
           src/xlets/parking \
           src/xlets/popcaastra
win32:SUBDIRS += src/xlets/outlook
