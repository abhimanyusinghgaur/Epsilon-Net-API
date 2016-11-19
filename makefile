#This is the makefile for this project.
#Use make command to compile this project using makefile

CC=g++

default:
	@echo "Building..."
	@$(CC) enet.cpp -o enet.out -std=c++11 -lCGAL -lgmp -frounding-math
	@echo "Build Successful!!!"
	@echo "Use ./enet.out to run."

#Use this rule to push from heroku server to git repo
git:
	git init
	git remote add origin https://github.com/abhimanyusinghgaur/Epsilon-Net-API.git
	git config --global user.email "abhimanyusinghgaur+heroku@gmail.com"
	git config --global user.name "Abhimanyu Singh Gaur Heroku"
	git add *
	git commit -m "Initial Commit"
	git push -f origin master

.PHONY: clean

clean:
	@rm enet.out
	@echo "Clean Complete!!!"