counter=1
until [ $counter -gt 50 ]
do
	./main -d ~/Desktop/training-typed/ -e 100 -l "saves/network_$counter.save"
	((counter++))
done

