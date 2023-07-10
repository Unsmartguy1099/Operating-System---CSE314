#!/bin/bash

roll=""

visit()
{ 
    IFS=$'\n'
    if [ $2 = true ]
    then
        roll=$1
    fi

	if [ -d $1 ]
	then

		for i in $1/*
		do
			visit "$i" false
		done
	
	elif [ -f $1 ]
	then
        echo test
		echo $i
        if [ ${i: -1} = "c" ]
            then
	            echo "C"
                mkdir -p ../Workspace/targets/C/$roll
                cp $i ../Workspace/targets/C/$roll/main.c  
        elif [ ${i: -1} = "y" ]
            then
	            echo "Py"
                mkdir -p ../Workspace/targets/Python/$roll
                cp $i ../Workspace/targets/Python/$roll/main.py
        elif [ ${i: -1} = "a" ]
            then
	            echo "Java"
                mkdir -p ../Workspace/targets/Java/$roll
                cp $i ../Workspace/targets/Java/$roll/Main.java
        fi
	fi
}

mkdir -p temp

cd ./Workspace/submissions
first_name=""

for i in `ls`
do
    if [ ${i: -3} = "zip" ]
        then 
        string=${i: -11}
        mkdir -p ../../temp/${string%.zip}
        #echo ${string%.zip}
        unzip $first_name\ $i -d ../../temp/${string%.zip}
    else
        first_name=$i
    fi
done

cd ../../
mkdir -p targets
mkdir -p ./targets/C
mkdir -p ./targets/Java
mkdir -p ./targets/Python
ls

cd ./temp
dir=true
for i in `ls`
do  
    visit $i true
done

