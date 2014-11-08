## Image to build from sources

FROM xivo/debian-qt5:latest
MAINTAINER XiVO Team "dev@avencall.com"

ENV DEBIAN_FRONTEND noninteractive
ENV HOME /root
ENV OUTPUT_DIR /output

# Add dependencies
RUN apt-get -qq update
RUN apt-get -qq -y install ssh apt-utils build-essential

# Configure environment
ADD baselib ${HOME}/
ADD packaging ${HOME}/
#ADD unit-tests ${HOME}/
ADD xivoclient ${HOME}/
ADD *.pr{i,o} ${HOME}/
ADD docker/home/.ssh/authorized_keys2 ${HOME}/.ssh/authorized_keys2
RUN chmod 600 ${HOME}/.ssh/authorized_keys2
RUN mkdir $OUTPUT_DIR

# Clean
RUN apt-get clean
RUN apt-get autoclean

# RUN
WORKDIR ${HOME}
ADD docker/run.sh ${HOME}/run.sh
RUN chmod +x ${HOME}/run.sh
CMD ${HOME}/run.sh
