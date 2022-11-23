rm -rf ./saves
mkdir saves
make clean
make
./main -d ~/Desktop/training_hand -e 5500
counter=1
until [ $counter -gt 250 ]
do
	echo "==="
	echo "Round $counter"
	./main -d ~/Desktop/training_hand/ -e 5500 -l "saves/network_$counter.save"
	echo "==="
	((counter++))
done
