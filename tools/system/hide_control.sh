#!/bin/bash
#-----------------------
# usage:
# this script is used for choosing to hide or nohide for some directory
# for example, we use private dir
#-----------------------

hideflag="false"
while true; do
	read -p "DO YOU WANT TO hide? [y/N]" yn
	case $yn in
	[Yy]* )
		chflags -R hidden private
		defaults write com.apple.finder AppleShowAllFiles -bool false
		hideflag="true"
		break;;
	[Nn]* ) 
		chflags -R nohidden private
		defaults write com.apple.finder AppleShowAllFiles -bool true
		hideflag="false"
		break;;
	* ) echo "Please answer yes or no.";;
	esac
done

if [ $hideflag = "true" ]; then
	echo "hide ok"
else
	echo "nohide ok"
fi

exit 0
