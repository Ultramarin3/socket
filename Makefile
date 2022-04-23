all:serverM serverA serverB serverC clientA clientB
.PHONY:all
	
serverM: serverM.c func.c func.h
	gcc -o serverM serverM.c func.c
serverA: serverA.c func.c func.h
	gcc -o serverA serverA.c func.c
serverB: serverB.c func.c func.h
	gcc -o serverB serverB.c func.c
serverC: serverC.c func.c func.h
	gcc -o serverC serverC.c func.c
clientA: clientA.c
	gcc -o clientA clientA.c
clientB: clientB.c
	gcc -o clientB clientB.c

clean:
	rm serverM serverA serverB serverC clientA clientB
