debian-setup:
	sudo apt update
	sudo apt install libasound2-dev libaubio-dev

build:
	gcc main.c -lasound -laubio -lm
