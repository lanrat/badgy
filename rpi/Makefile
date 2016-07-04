CC=gcc 
CFLAGS=-Wall -lwiringPi

all: badgy

install: badge.service
	ln -f -s /root/badgy/badge.service /etc/systemd/system/badge.service
	ln -f -s /etc/systemd/system/badge.service /etc/systemd/system/multi-user.target.wants/clock.service
	systemctl daemon-reload

badgy: badgy.c