import numpy as np

myList = list(range(1, 101))

np.random.shuffle(myList)

for i in myList:
    print("INSERT",i)

for i in range(1, 100):
    print("RANGE",i,i+1)